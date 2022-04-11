#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdlib.h>
#include <functional>
#include <map>
#include <unistd.h>
#include <iostream>



#include "../include/cache.h"

using namespace std;
using namespace cacheSim;
/** @brief Parses command line args */
int parameterParser(int argc, char *argv[], int *s, int *E, int *b,
                     char **traceFilePtr);

/** @brief Parses through the trace file */
int traceParser(char *traceFile, int s, int E, int b, cache *cacheCore);

int main(int argc, char *argv[]) {
    int S, s, E, b, B, retValue;
    char *traceFile;
    /* Parse command line arguments */
    retValue = parameterParser(argc, argv, &s, &E, &b, &traceFile);
    if (retValue < 0) { 
        return (-1);
    }
    S = 1 << s;
    B = 1 << b;
    //cout << "s:" << s << " S:" << S << " E:" << E << " b:" << b << " t:" << traceFile << endl;
    printf("S:%d E:%d B:%d \n", S, E, B);
    cache *cacheCore0 = new cache(S, E, B);

    retValue = traceParser(traceFile, s, E, b, cacheCore0);
    if (retValue < 0) {
        delete cacheCore0;
        return (-1);
    }
}

/*
 * @brief Parses command line arguments
 *
 * This function parses command line arguments and checks if the input given
 * are valid.
 * @return -1 if incorrect arguments, 0 if OK.
 */
int parameterParser(int argc, char *argv[], int *s, int *E, int *b,
                     char **traceFilePtr) {
    extern char *optarg;
    char arg;
    int errFlag = 0;
    char *optstring = (char *)"s:E:b:t:";
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
        case 't':
            *traceFilePtr = optarg;
            break;
        default:
            errFlag = 1;
            break;
        }
    }
    /* Check for invalid args */
    if (errFlag || *s < 0 || *E <= 0 || *b < 0 || traceFilePtr == NULL) {
        return (-1);
    }
    return (0);
}

/*
 * @brief Parses through the trace file
 *
 * This function parses through the trace file and gets the operation, memory
 * and the size in each parse. It then calls the address parser which gives
 * the set_no and the tag data of the line. It eventually calls the cache
 * logic block which checks for hits,misses and evictions.
 *
 * @return -1 if the file is not present, 0 if everything is good.
 */
int traceParser(char *traceFile, int s, int E, int b, cache* cacheCore) {
    FILE *handle;
    char operation;
    long address;
    int size;
    //cout << traceFile << endl;
    handle = fopen(traceFile, "r");
    if (handle == NULL) {
        cout << "file open failed" << endl;
        return (-1);
    }
    cout << "here" << endl;
    while (fscanf(handle, "%c %lx,%d\n", &operation, &address, &size) > 0) {
       cacheCore->cacheLogic(operation, address);
    }
    fclose(handle);
    return (0);
}

