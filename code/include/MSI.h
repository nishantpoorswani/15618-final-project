#ifndef MSI_H
#define MSI_H

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
    class MSI
    {
        public:
            unsigned long busReqs;
            unsigned long busRdXReqs;
            unsigned long invalidations;
            enum lineState
            {
                invalid = 0,
                shared = 1,
                modified = 2
            };

            enum busAction
            {
                noBusAction = 0,
                busRd = 1,
                busRdX = 2
            };

            enum prAction
            {
                prRd = 0,
                prWr = 1
            };

        void controller(int numCores, cache** cacheCore, int tid, char operation, long address);

        busAction handleProcessorAction(cacheSim::cache **cacheCore, int tid, char operation, long address, prAction prAc);

        void handleBusAction(int numCores, cacheSim::cache **cacheCore, int tid, busAction busAc, long address);
    };
}

#endif /* MSI_H */