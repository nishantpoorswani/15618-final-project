#include "../include/MSI.h"

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

    void MSI::handleProcessorAction(cacheSim::cache **cacheCore, int tid, char operation, long address)
    {
        /* Update cache for the requested memory event for a particular cache */
        cacheCore[tid]->cacheLogic(operation, (long)address);

        /* Update the state of the cache line to modified */
        auto& caSet = cacheCore[tid]->getLine(address);
        std::list<cacheSim::cache::cacheLine>::iterator line = caSet.end();
        //std::cout << "Inside processor action after end size: " << caSet.size() << "\n";
        line--;
        //std::cout << "Inside processor action after decrement iterator: address: " << line->addr << " " << line->tag << " " << line->state << "\n";
        if(operation == 'S')
        {
            line->state = (int)modified;
        }
        else if(operation == 'L' && line->state == (int)invalid)
        {
            line->state = (int)shared;            
        }
        //std::cout << "Inside processor action after change line state: address: " << line->addr << " " << line->tag << " " << line->state << "\n";
    }

    void MSI::handleBusAction(int numCores, cacheSim::cache **cacheCore, int tid,prAction prAc, long address)
    {
        /* Based on the processor action, generate a bus action 
        and update the cache line state in other processor caches */
        busAction busAc;
        busAc = (prAc == prRd) ? busRd : busRdX; 

        for(int i=0; i < numCores ; i++)
        {
            if(i != tid)
            {
                auto& caSet = cacheCore[i]->getLine(address);
                auto line = find_if(caSet.begin(), caSet.end(),
                    [address, cacheCore, i](cacheSim::cache::cacheLine l){return (l.tag == cacheCore[i]->getTag(address));});
                if (line == caSet.end()) {
                    continue;
                }
                //std::cout << "Inside bus action: address: " << address << " tid: " << tid << "\n";
                /* Update the state of the cache line in other caches to invalid if busAc is busRdX */
                if(busAc == busRdX)
                {
                    line->state = (int)invalid;
                    caSet.erase(line);
                }
                else
                {
                    line->state = (int)shared;
                }
            }
        }  
    }

    void MSI::controller(int numCores, cacheSim::cache **cacheCore, int tid, char operation, long address)
    {
        //std::cout << "address: " << address << " tid: " << tid << "\n";
        prAction prAc; 
        
        handleProcessorAction(cacheCore, tid, operation, address);

        prAc = (operation == 'L') ? prRd : prWr;

        handleBusAction(numCores, cacheCore, tid, prAc, address);

        //cacheCore[0]->cacheLogic(operation, (long)address);
    }
}
