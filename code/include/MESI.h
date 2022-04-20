#ifndef MESI_H
#define MESI_H

#include <algorithm>
#include <cstdio>
#include <cassert>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <list>
#include <iostream>

#include "../include/cache.h"

namespace cacheSim
{
    class MESI
    {
        public:
            enum lineState
            {
                invalid = 0,
                modified = 1
            };

            enum busAction
            {
                busRd = 0,
                busWr = 1
            };

            enum prAction
            {
                prRd = 0,
                prWr = 1
            };

        void controller(int numCores, cache** cacheCore, int tid, char operation, long address);

        void handleProcessorAction(cacheSim::cache **cacheCore, int tid, char operation, long address);

        void handleBusAction(int numCores, cacheSim::cache **cacheCore, int tid,prAction prAc, long address);
    };
}

#endif /* MESI_H */