#include <algorithm>
#include <cstdio>
#include <cassert>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <list>

#include "../include/cache.h"

namespace cacheSim
{
    class MI
    {
        public:
            enum lineState
            {
                modified = 0,
                invalid = 1
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

        void controller(cache** cacheCore, char operation, long address, prAction prAc, busAction busAc);

    };
}