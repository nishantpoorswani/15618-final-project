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
                shared = 1,
                modified = 2,
                exclusive = 3
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

        busAction handleProcessorAction(int numCores, cacheSim::cache **cacheCore, int tid, char operation, long address, prAction prAc);

        void handleBusAction(int numCores, cacheSim::cache **cacheCore, int tid, busAction busAc, long address);
    };
}

#endif /* MESI_H */