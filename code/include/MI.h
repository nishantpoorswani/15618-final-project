#ifndef MI_H
#define MI_H

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
    class MI
    {
        public:
            unsigned long busReqs;
            unsigned long busRdXReqs;
            unsigned long invalidations;
            enum lineState
            {
                invalid = 0,
                modified = 1
            };

            enum busAction
            {
                noBusAction = 0,
                busRd = 1,
                busWr = 2
            };

            enum prAction
            {
                prRd = 0,
                prWr = 1
            };

        void controller(int numCores, cache** cacheCore, int tid, char operation, long address);

        busAction handleProcessorAction(cacheSim::cache **cacheCore, int tid, char operation, long address, prAction prAc);

        void handleBusAction(int numCores, cacheSim::cache **cacheCore, int tid,long address, busAction busAc);
    };
}

#endif /* MI_H */