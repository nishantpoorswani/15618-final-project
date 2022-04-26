/*
 * Copyright (C) 2004-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

/*
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <functional>
#include <map>
#include <unistd.h>
#include <iostream>

#include <stdio.h>
#include "pin.H"
#include "tp.h"

#include "../include/MI.h"
#include "../include/MSI.h"
#include "../include/MESI.h"

FILE* trace;
PIN_LOCK lock;
/* number of cores */
int numCores;
/* Pointer to multi-core cache objects */
cacheSim::cache **cacheCore = NULL;
/* Snooping based cache protocol objects */
cacheSim::MI MIProtocol;
cacheSim::MSI MSIProtocol;
cacheSim::MESI MESIProtocol;
/* Character array used to hold the protocol which is being used */
char protocol[10] = {0};


// Print a memory read record
VOID RecordMemRead(VOID* ip, VOID* addr, THREADID tid) { 
    PIN_GetLock(&lock, tid);
    //fprintf(trace, "%p: R %p %d\n", ip, addr, tid);
    //fprintf(trace, "L %lx,1\n", (long int)addr);
    //cacheCore[0]->cacheLogic('L', (long)addr); 
    if(!strncmp(protocol, "MI", sizeof("MI")))
    {
        MIProtocol.controller(numCores, cacheCore, tid, 'L', (long)addr);
    }
    else if(!strncmp(protocol, "MSI", sizeof("MSI")))
    {
        MSIProtocol.controller(numCores, cacheCore, tid, 'L', (long)addr);
    }
    else if(!strncmp(protocol, "MESI", sizeof("MESI")))
    {
        MESIProtocol.controller(numCores, cacheCore, tid, 'L', (long)addr);
    }
    PIN_ReleaseLock(&lock);
}

// Print a memory write record
VOID RecordMemWrite(VOID* ip, VOID* addr, THREADID tid) {
    PIN_GetLock(&lock, tid); 
    //fprintf(trace, "S %lx,1\n", (long int)addr);
    //cacheCore[0]->cacheLogic('S', (long)addr);
    if(!strncmp(protocol, "MI", sizeof("MI")))
    {
        MIProtocol.controller(numCores, cacheCore, tid, 'S', (long)addr);
    }
    else if(!strncmp(protocol, "MSI", sizeof("MSI")))
    {
        MSIProtocol.controller(numCores, cacheCore, tid, 'S', (long)addr);
    }
    else if(!strncmp(protocol, "MESI", sizeof("MESI")))
    {
        MESIProtocol.controller(numCores, cacheCore, tid, 'S', (long)addr);
    }
    PIN_ReleaseLock(&lock);
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID* v)
{
    // Instruments memory accesses using a predicated call, i.e.
    // the instrumentation is called iff the instruction will actually be executed.
    //
    // On the IA-32 and Intel(R) 64 architectures conditional moves and REP
    // prefixed instructions appear as predicated instructions in Pin.
    UINT32 memOperands = INS_MemoryOperandCount(ins);

    // Iterate over each memory operand of the instruction.
    for (UINT32 memOp = 0; memOp < memOperands; memOp++)
    {
        if (INS_MemoryOperandIsRead(ins, memOp))
        {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp,
                                     IARG_THREAD_ID, IARG_END);
        }
        // Note that in some architectures a single memory operand can be
        // both read and written (for instance incl (%eax) on IA-32)
        // In that case we instrument it once for read and once for write.
        if (INS_MemoryOperandIsWritten(ins, memOp))
        {
            INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite, IARG_INST_PTR, IARG_MEMORYOP_EA, memOp,
                                     IARG_THREAD_ID, IARG_END);
        }
    }
}

VOID Fini(INT32 code, VOID* v)
{
    //fprintf(trace, "#eof\n");
    FILE * output;
    output = fopen("output.csv", "w");
    fprintf(output, "Cache_ID,Hits,Misses,Evictions\n");
    for(int i = 0; i < numCores; i++)
    {
        printf("hits:%d, misses:%d, evictions:%d \n", cacheCore[i]->hits, cacheCore[i]->misses, cacheCore[i]->evictions);
        fprintf(output, "%d,%d,%d,%d\n", i, cacheCore[i]->hits, cacheCore[i]->misses, cacheCore[i]->evictions);
        delete cacheCore[i];
    }
    fclose(output);
    delete cacheCore;
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR("This Pintool prints a trace of memory addresses\n" + KNOB_BASE::StringKnobSummary() + "\n");
    return -1;
}

/*
 * @brief Parses command line arguments
 *
 * This function parses command line arguments and checks if the input given
 * are valid.
 * @return -1 if incorrect arguments, 0 if OK.
 */
int parameterParser(int argc, char *argv[], int *s, int *E, int *b) {
    extern char *optarg;
    char arg;
    int errFlag = 0;
    char *optstring = (char *)"s:E:b:";
    /* Parse command line args */
    while ((arg = getopt(argc, argv, optstring)) != -1) {
        switch (arg) {
        case 's':
            *s = atoi(optarg);
            break;
        case 'E':
            *E = atoi(optarg);
            break;
        case 'b':
            *b = atoi(optarg);
            break;
        default:
            errFlag = 1;
            break;
        }
    }
    /* Check for invalid args */
    if (errFlag || *s < 0 || *E <= 0 || *b < 0 ) {
        return (-1);
    }
    return (0);
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char* argv[])
{
    int S, s, E, b, B;// retValue;
    FILE * fptr;
    fptr = fopen("cacheSim.config", "r");
    fscanf(fptr, "%*s %d", &s);
    fscanf(fptr, "%*s %d", &E);
    fscanf(fptr, "%*s %d", &b);
    fscanf(fptr, "%*s %s", protocol);
    fscanf(fptr, "%*s %d", &numCores);
    fclose(fptr);
    printf("s:%d E:%d b:%d protocol:%s numCores:%d\n",s, E, b, protocol, numCores);
    // s=5;
    // E=1;
    // b=5;
    /* Parse command line arguments */
    // retValue = parameterParser(argc, argv, &s, &E, &b);
    // if (retValue < 0) { 
    //     return (-1);
    // }
    S = 1 << s;
    B = 1 << b;

    cacheCore = new cacheSim::cache*[numCores];

    for(int i=0; i<numCores; i++)
    {
        cacheCore[i] = new cacheSim::cache(S, E, B);
    }
    //printf("S:%d E:%d B:%d \n", S, E, B);
    //cacheCore0 = new cacheSim::cache(S, E, B);
    
    if (PIN_Init(argc, argv)) return Usage();

    //trace = fopen("pinatrace.out", "w");

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}
