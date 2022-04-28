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
#include <vector>
#include <string>
#include <fstream>

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
std::vector<std::string> userFuncs;

typedef struct {
    std::string funcName;
    unsigned long beforeEvic[3];
    unsigned long beforeFuncHits[3];
    unsigned long beforeFuncMisses[3];
    unsigned long hits;
    unsigned long misses;
    unsigned long evictions;
} funcStats_t;

/* Pointer to function stats */
funcStats_t **funcStats = NULL;

// This function is called before every instruction is executed
VOID funcEntry(std::string funcName, THREADID tid) {  
    PIN_GetLock(&lock, tid);
    //std::cout << "Before func:" << funcName << " " << tid << std::endl;
    for(unsigned int i=0; i<userFuncs.size(); i++)
    {
        if(!funcName.compare(funcStats[i]->funcName))
        {
            std::cout << "Index" << i << " Before func:" << funcName << " " << tid << std::endl;
            funcStats[i]->beforeFuncHits[tid] = cacheCore[tid]->hits;
            funcStats[i]->beforeFuncMisses[tid] = cacheCore[tid]->misses;
            std::cout << "Reached before in entry" << std::endl;
            funcStats[i]->beforeEvic[tid] = cacheCore[tid]->evictions;
            std::cout << "Reached after in entry" << std::endl;
            PIN_ReleaseLock(&lock);
            return;
        }
    }
    PIN_ReleaseLock(&lock);
}
 
// This function is called after every instruction is executed
VOID funcExit(std::string funcName, THREADID tid) { 
    PIN_GetLock(&lock, tid);    
    //std::cout << "After func:" << funcName << " " << tid << std::endl;
    for(unsigned int i=0; i<userFuncs.size(); i++)
    {
        if(!funcName.compare(funcStats[i]->funcName))
        {
            std::cout << "Index" << i << " After func:" << funcName << " " << tid << std::endl;
            funcStats[i]->hits += cacheCore[tid]->hits - funcStats[i]->beforeFuncHits[tid];
            funcStats[i]->misses += cacheCore[tid]->misses  - funcStats[i]->beforeFuncMisses[tid];
            std::cout << "Reached before in exit" << funcName <<std::endl;
            funcStats[i]->evictions += cacheCore[tid]->evictions - funcStats[i]->beforeEvic[tid];
            std::cout << "Reached after in exit" << funcName << std::endl;
            PIN_ReleaseLock(&lock);
            return;
        }
    }
    PIN_ReleaseLock(&lock);
}

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

VOID Routine(RTN rtn, VOID *v) {
    std::string name = PIN_UndecorateSymbolName(RTN_Name(rtn).c_str(),
                       UNDECORATION_NAME_ONLY);
    std::vector<std::string>::iterator it;
    std::cout << "RTN_NAME:" <<RTN_Name(rtn) << std::endl;
    //std::cout << "Func_name after verification: " << userFunc << " and name obtained:" << name << std::endl;
    RTN_Open(rtn);
    // Insert a call at the entry point of a routine to increment the call count
    RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)funcEntry, IARG_PTR, new std::string(name), IARG_THREAD_ID, IARG_END);
    RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)funcExit, IARG_PTR, new std::string(name), IARG_THREAD_ID, IARG_END);
    // For each instruction of the routine
    for (INS ins = RTN_InsHead(rtn); INS_Valid(ins); ins = INS_Next(ins)) 
    {
        UINT32 memOperands = INS_MemoryOperandCount(ins);
        // Iterate over each memory operand of the instruction.
        for (UINT32 memOp = 0; memOp < memOperands; memOp++) {
            if (INS_MemoryOperandIsRead(ins, memOp)) 
            {
                INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp, IARG_THREAD_ID,
                IARG_END);
            }
            if (INS_MemoryOperandIsWritten(ins, memOp)) 
            {
                INS_InsertPredicatedCall(
                ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
                IARG_INST_PTR,
                IARG_MEMORYOP_EA, memOp, IARG_THREAD_ID,
                IARG_END);
            }
        }
    }
    RTN_Close(rtn);
}

VOID Fini(INT32 code, VOID* v)
{
    //fprintf(trace, "#eof\n");
    printf("Inside fini\n");
    FILE * output;
    output = fopen("output.csv", "w");
    fprintf(output, "Cache_ID,Hits,Misses,Evictions\n");
    for(int i = 0; i < numCores; i++)
    {
        printf("hits:%lu, misses:%lu, evictions:%lu \n", cacheCore[i]->hits, cacheCore[i]->misses, cacheCore[i]->evictions);
        fprintf(output, "%d,%lu,%lu,%lu\n", i, cacheCore[i]->hits, cacheCore[i]->misses, cacheCore[i]->evictions);
        delete cacheCore[i];
    }
    for(unsigned int i=0; i < userFuncs.size(); i++)
    {
        // delete funcStats[i]->beforeFuncHits;
        // delete funcStats[i]->beforeFuncMisses;
        // delete funcStats[i]->beforeEvic;
        delete funcStats[i];
    }
    fclose(output);
    delete cacheCore;
    delete funcStats;
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

    S = 1 << s;
    B = 1 << b;

    cacheCore = new cacheSim::cache*[numCores];

    for(int i=0; i<numCores; i++)
    {
        cacheCore[i] = new cacheSim::cache(S, E, B);
    }

    PIN_InitSymbols(); 
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("pinatrace.out", "w");
    std::ifstream configFile;
    configFile.open("userFuncs.in");

    std::string st;
    while (std::getline(configFile, st)) {
        userFuncs.push_back((st));
    }

    std::cout << userFuncs.size() << std::endl;

    funcStats = new funcStats_t*[userFuncs.size()];

    for(unsigned int i=0; i < userFuncs.size(); i++)
    {
        funcStats[i] = new funcStats_t;
        // funcStats[i]->beforeEvic = new unsigned long (numCores);
        // funcStats[i]->beforeFuncHits = new unsigned long (numCores);
        // funcStats[i]->beforeFuncMisses = new unsigned long (numCores);
        funcStats[i]->funcName = userFuncs[i];
    }

    configFile.close();
    RTN_AddInstrumentFunction(Routine, 0);    

    //INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();

    return 0;
}
