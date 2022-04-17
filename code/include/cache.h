#include <algorithm>
#include <cstdio>
#include <cassert>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <queue>
#include <list>

namespace cacheSim
{
    class cache 
    {
        public:
            int hits;
            int misses;
            int evictions;
            struct cacheLine {
                long addr;  //add based on usage
                long tag;
                /* Cache line state to implement snooping based coherence protocols */
                int state;
                cacheLine(long addr, long tag):
                    addr(addr), tag(tag) {}
            };

            cache(int sets, int associativity, int blockSize);

            void cacheLogic(char operation, long address);

        protected:
            unsigned int sets;
            unsigned int associativity;
            unsigned int blockNum;
            unsigned int indexMask;
            unsigned int blockSize;
            unsigned int indexOffset;
            unsigned int tagOffset;

            std::map<int, std::list<cacheLine> > cacheSet;

            int calcLog2(int val) 
            {
                int n = 0;
                while ((val >>= 1))
                    n ++;
                return n;
            }

            int getIndex(long addr) 
            {
                return (addr >> indexOffset) & indexMask;
            };

            long getTag(long addr) 
            {
                return (addr >> tagOffset);
            }

            long align(long addr) 
            {
                return (addr & ~(blockSize-1l));
            }

            void evict(std::list<cacheLine>* line, std::list<cacheLine>::iterator victim);

            void allocateLine(std::list<cacheLine>& lines, long addr, int coreid);

            bool needEviction(const std::list<cacheLine>& lines, long addr);
            
            bool isHit(std::list<cacheLine>& line, long addr, std::list<cacheLine>::iterator* posPtr);

            std::list<cacheLine>& getLine(long addr) 
            {
                if (cacheSet.find(getIndex(addr))
                    == cacheSet.end()) 
                {
                    cacheSet.insert(make_pair(getIndex(addr), std::list<cacheLine>()));
                }
                return cacheSet[getIndex(addr)];
            }

    };
} // namespace cacheSim