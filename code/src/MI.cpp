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

    void MI::handleProcessorAction(cacheSim::cache **cacheCore, int tid, char operation, long address)
    {
        /* Update cache for the requested memory event for a particular cache */
        cacheCore[tid]->cacheLogic(operation, (long)address);

        /* Update the state of the cache line to modified */
        auto& caSet = cacheCore[tid]->getLine(address);
        std::list<cacheSim::cache::cacheLine>::iterator line = caSet.end();
        line--;
        line->state = (int)modified;
    }

    void MI::handleBusAction(int numCores, cacheSim::cache **cacheCore, int tid,prAction prAc, long address)
    {
        /* Based on the processor action, generate a bus action 
        and update the cache line state in other processor caches */
        busAction busAc;
        busAc = (prAc == prRd) ? busRd : busWr; 

        if(busAc == busWr)
        {
            for(int i=0; i < numCores ; i++)
            {
                if(i != tid)
                {
                    /* Update the state of the cache line in other caches to invalid */
                    auto& caSet = cacheCore[i]->getLine(address);
                    auto line = find_if(caSet.begin(), caSet.end(),
                        [address, cacheCore, i](cacheSim::cache::cacheLine l){return (l.tag == cacheCore[i]->getTag(address));});
                    if (line == caSet.end()) {
                        continue;
                    }
                    line->state = (int)invalid;
                    caSet.erase(line);
                }
            }
        }   
    }

    void MI::controller(int numCores, cacheSim::cache **cacheCore, int tid, char operation, long address)
    {
        
        prAction prAc; 
        
        handleProcessorAction(cacheCore, tid, operation, address);

        prAc = (operation == 'L') ? prRd : prWr;

        handleBusAction(numCores, cacheCore, tid, prAc, address);

        //cacheCore[0]->cacheLogic(operation, (long)address);
    }
}