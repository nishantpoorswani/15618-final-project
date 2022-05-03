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

    MI::busAction MI::handleProcessorAction(cacheSim::cache **cacheCore, int tid, char operation, long address, prAction prAc)
    {
        lineState prevLineState;
        busAction busAc = noBusAction;

        /* Update cache for the requested memory event for a particular cache */
        cacheCore[tid]->cacheLogic(operation, (long)address);

        /* Update the state of the cache line to modified */
        auto& caSet = cacheCore[tid]->getLine(address);
        std::list<cacheSim::cache::cacheLine>::iterator line = caSet.end();
        line--;

        /* Get the previous line state */
        prevLineState = (lineState) line->state;

        if(prAc == prRd)
        {
            if (prevLineState == invalid)
            {
                busAc = busRd;
                busReqs++;
                line->state = (int)modified;
            }
            else
            {
                /*Line is expected to be in modified state */
                /* Can add an assert here to check above */
            }
        }
        if(prAc == prWr)
        {
            if (prevLineState == invalid)
            {
                busAc = busWr;
                busReqs++;
                busRdXReqs++;
                line->state = (int)modified;
            }
            else
            {
                /*Line is expected to be in modified state */
                /* Can add an assert here to check above */
                busAc = busWr;
                busReqs++;
                busRdXReqs++;
            }    
        }

        return busAc;
        //std::cout << "Inside processor action after change line state: address: " << line->addr << " " << line->tag << " " << line->state << "\n";
    }

    void MI::handleBusAction(int numCores, cacheSim::cache **cacheCore, int tid, long address, busAction busAc)
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
                //std::cout << "Inside bus action: address: " << address << " tid: " << tid << "\n"; 
                if(busAc == busWr)
                {
                    line->state = (int)invalid;
                    caSet.erase(line);
                    invalidations++;
                }
                else
                {
                    /* Need not change the line state if it's a busRd */
                }
            }
        } 
    }

    void MI::controller(int numCores, cacheSim::cache **cacheCore, int tid, char operation, long address)
    {
        //std::cout << "address: " << address << " tid: " << tid << "\n";
        prAction prAc;  //processor action
        busAction busAc = noBusAction; //bus action

        prAc = (operation == 'L') ? prRd : prWr;
        
        busAc = handleProcessorAction(cacheCore, tid, operation, address, prAc);

        handleBusAction(numCores, cacheCore, tid, address, busAc);

        //cacheCore[0]->cacheLogic(operation, (long)address);
    }
}