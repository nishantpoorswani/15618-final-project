#include "../include/MESI.h"

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

    MESI::busAction MESI::handleProcessorAction(int numCores, cacheSim::cache **cacheCore, int tid, char operation, long address, prAction prAc)
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

        /* If the cache line is not present in the processor's cache and is a processor read */
        if((prAc == prRd) && (prevLineState == invalid)) 
        {   
            bool linePresent = false;
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
                    linePresent = true;
                    break;
                }
            }
            //If the cache line is present in another processor's cache update the cache line to be shared
            if (linePresent ==true)
            {
                line->state = (int)shared;
            }
            else //If the cache line is not present in another processor's cache update the cache line to be exclusive
            {
                line->state =(int) exclusive;
            }
            busAc = busRd;
            busReqs++;
        }
        
        /* If the cache line is present in the processor's cache and 
          is a processor read, need not update the cache line state */

        /* If the current request is a processor write */
        if(prAc == prWr)
        {
            /* If the previous cache line state is in invalid or shared state
               it is updated to modified and genrate a read exclusive bus action */
            if((prevLineState == invalid) || (prevLineState == shared))
            {
                line->state = (int)modified;
                busAc = busRdX;
                busRdXReqs++;
                busReqs++;
            }
            /* If the previous cache line state is in exclusive state
               it is updated to modified and do not generate a bus action */
            else if(prevLineState == exclusive)
            {
                line->state =(int)modified;
                busAc = noBusAction;
            }
            /* If the If the previous cache line state is in modified state,
               do not update the cache line state or generate a bus action */
        }
        
        return busAc;
    }

    void MESI::handleBusAction(int numCores, cacheSim::cache **cacheCore, int tid, busAction busAc, long address)
    {
        for(int i=0; i < numCores ; i++)
        {
            if(i != tid)
            {
                lineState prevLineState;

                auto& caSet = cacheCore[i]->getLine(address);
                auto line = find_if(caSet.begin(), caSet.end(),
                    [address, cacheCore, i](cacheSim::cache::cacheLine l){return (l.tag == cacheCore[i]->getTag(address));});
                if (line == caSet.end()) {
                    continue;
                }
                /* Get the previous line state */
                prevLineState = (lineState) line->state;
                /* If the bus action is bus read
                    update the cache line state to shared */
                if(busAc == busRd)
                {
                    if((prevLineState == exclusive) || (prevLineState == modified))
                    {
                        line->state = (int)shared;
                    }
                    else{
                        //Add an assert to check if the cache line is in shared state
                        /* Cache line state is shared and it has 
                           to remain in the sahared state */
                    }
                }
                /* If the bus action is bus read exclusive,
                  update the cache line to be invalid and remove
                  the cache line from the cache */
                if(busAc == busRdX)
                { //Add an assert to check if the previous line is not invalid
                    line->state = (int)invalid;
                    caSet.erase(line);
                    invalidations++; 
                }
            }
        } 
    }

    void MESI::controller(int numCores, cacheSim::cache **cacheCore, int tid, char operation, long address)
    {
        prAction prAc;  //processor action
        busAction busAc = noBusAction; //bus action
        
        prAc = (operation == 'L') ? prRd : prWr;

        busAc = handleProcessorAction(numCores, cacheCore, tid, operation, address, prAc);

        handleBusAction(numCores, cacheCore, tid, busAc, address);
    }
}
