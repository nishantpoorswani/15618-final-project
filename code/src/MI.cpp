#include "../include/MI.h"

//#define DEBUG_CACHE

#ifndef DEBUG_CACHE
#define debug(...)
#else
#define debug(...) do { \
          printf("\033[36m[DEBUG] %s ", __FUNCTION__); \
          printf(__VA_ARGS__); \
          printf("\033[0m\n"); \
      } while (0)
#endif

namespace cacheSim
{
    void MI::controller(cacheSim::cache **cacheCore, char operation, long address, prAction prAc, busAction busAc)
    {
        cacheCore[0]->cacheLogic(operation, (long)address);
    }
}