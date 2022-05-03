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
    unsigned long *beforeEvic;
    unsigned long *beforeFuncHits;
    unsigned long *beforeFuncMisses;
    unsigned long hits;
    unsigned long misses;
    unsigned long evictions;
} funcStats_t;

/* Pointer to function stats */
funcStats_t **funcStats = NULL;

// This function is called before every instruction is executed
VOID funcEntry(std::string funcName, THREADID tid) {  
    PIN_GetLock(&lock, tid);
    for(unsigned int i=0; i<userFuncs.size(); i++)
    {
        if(!funcName.compare(funcStats[i]->funcName))
        {
            funcStats[i]->beforeFuncHits[tid] = cacheCore[tid]->hits;
            funcStats[i]->beforeFuncMisses[tid] = cacheCore[tid]->misses;
            funcStats[i]->beforeEvic[tid] = cacheCore[tid]->evictions;
            PIN_ReleaseLock(&lock);
            return;
        }
    }
    PIN_ReleaseLock(&lock);
}
 
// This function is called after every instruction is executed
VOID funcExit(std::string funcName, THREADID tid) { 
    PIN_GetLock(&lock, tid);    
    for(unsigned int i=0; i<userFuncs.size(); i++)
    {
        if(!funcName.compare(funcStats[i]->funcName))
        {
            funcStats[i]->hits += cacheCore[tid]->hits - funcStats[i]->beforeFuncHits[tid];
            funcStats[i]->misses += cacheCore[tid]->misses  - funcStats[i]->beforeFuncMisses[tid];
            funcStats[i]->evictions += cacheCore[tid]->evictions - funcStats[i]->beforeEvic[tid];
            PIN_ReleaseLock(&lock);
            return;
        }
    }
    PIN_ReleaseLock(&lock);
}

// Print a memory read record
VOID RecordMemRead(VOID* ip, VOID* addr, THREADID tid) { 
    PIN_GetLock(&lock, tid);
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
        //std::cout<<"MESI called L" << std::endl; 
        MESIProtocol.controller(numCores, cacheCore, tid, 'L', (long)addr);
    }
    PIN_ReleaseLock(&lock);
}

// Print a memory write record
VOID RecordMemWrite(VOID* ip, VOID* addr, THREADID tid) {
    PIN_GetLock(&lock, tid); 
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
        //std::cout<<"MESI called S" << std::endl;
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
    // std::cout << "RTN_NAME:" <<RTN_Name(rtn) << std::endl;
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
    FILE * output;
    output = fopen("output.csv", "w");
    fprintf(output, "Cache_ID,Hits,Misses,Evictions\n");
    for(int i = 0; i < numCores; i++)
    {
        printf("Thread_ID:%d,hits:%lu, misses:%lu, evictions:%lu \n", i, cacheCore[i]->hits, cacheCore[i]->misses, cacheCore[i]->evictions);
        fprintf(output, "%d,%lu,%lu,%lu\n", i, cacheCore[i]->hits, cacheCore[i]->misses, cacheCore[i]->evictions);
        delete cacheCore[i];
    }
    fclose(output);
    FILE * output_functions;
    output_functions = fopen("output_funcs.csv", "w");
    fprintf(output_functions, "Func_name,Hits,Misses,Evictions\n");
    for(unsigned int i=0; i < userFuncs.size(); i++)
    {
        for(int j=1; j < numCores; j++)
        {
            funcStats[i]->beforeFuncHits[0] += funcStats[i]->beforeFuncHits[j];
            funcStats[i]->beforeFuncMisses[0] += funcStats[i]->beforeFuncMisses[j];
            funcStats[i]->beforeEvic[0] += funcStats[i]->beforeEvic[j];
        }
        fprintf(output_functions, "%s,%lu,%lu,%lu\n", funcStats[i]->funcName.c_str(), funcStats[i]->beforeFuncHits[0], funcStats[i]->beforeFuncMisses[0], funcStats[i]->beforeEvic[0]);
        printf("%s,%lu,%lu,%lu\n", funcStats[i]->funcName.c_str(), funcStats[i]->beforeFuncHits[0], funcStats[i]->beforeFuncMisses[0], funcStats[i]->beforeEvic[0]); 
        delete funcStats[i]->beforeFuncHits;
        delete funcStats[i]->beforeFuncMisses;
        delete funcStats[i]->beforeEvic;
        delete funcStats[i];
    }
    FILE * output_traffic;
    output_traffic = fopen("output_traffic.csv", "w");
    fprintf(output_traffic, "Bus Requests,BusRdX,Invalidations\n");
    if(!strncmp(protocol, "MI", sizeof("MI")))
    {
        fprintf(output_traffic, "%lu,%lu,%lu\n", MIProtocol.busReqs, MIProtocol.busRdXReqs, MIProtocol.invalidations);
        printf("MI: Bus Requests:%lu, Exclusive bus reads:%lu, invalidations:%lu \n", MIProtocol.busReqs, MIProtocol.busRdXReqs, MIProtocol.invalidations);
    }
    else if(!strncmp(protocol, "MSI", sizeof("MSI")))
    {
        fprintf(output_traffic, "%lu,%lu,%lu\n", MSIProtocol.busReqs, MSIProtocol.busRdXReqs, MSIProtocol.invalidations);
        printf("MSI: Bus Requests:%lu, Exclusive bus reads:%lu, invalidations:%lu \n", MSIProtocol.busReqs, MSIProtocol.busRdXReqs, MSIProtocol.invalidations);
    }
    else if(!strncmp(protocol, "MESI", sizeof("MESI")))
    {
        fprintf(output_traffic, "%lu,%lu,%lu\n", MESIProtocol.busReqs, MESIProtocol.busRdXReqs, MESIProtocol.invalidations);
        printf("MESI: Bus Requests:%lu, Exclusive bus reads:%lu, invalidations:%lu \n", MESIProtocol.busReqs, MESIProtocol.busRdXReqs, MESIProtocol.invalidations);
    }
    fclose(output_functions);
    fclose(output_traffic);
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
    /* Init Symbols (Needed for functions) */
    PIN_InitSymbols(); 
    if (PIN_Init(argc, argv)) return Usage();

    trace = fopen("pinatrace.out", "w");
    std::ifstream configFile;
    configFile.open("userFuncs.in");

    std::string st;
    while (std::getline(configFile, st)) {
        userFuncs.push_back((st));
    }
    funcStats = new funcStats_t*[userFuncs.size()];

    for(unsigned int i=0; i < userFuncs.size(); i++)
    {
        funcStats[i] = new funcStats_t;
        funcStats[i]->beforeEvic = new unsigned long[numCores];
        funcStats[i]->beforeFuncHits = new unsigned long[numCores];
        funcStats[i]->beforeFuncMisses = new unsigned long[numCores];
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
