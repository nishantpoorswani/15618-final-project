#include "../include/cache.h"
#include <iostream>

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
  cache::cache(int sets, int associativity, int blockSize):
      sets(sets), associativity(associativity),
      blockSize(blockSize)
  {
    debug("sets %d associativity %d blockSize %d\n",
        sets, associativity, blockSize);

    int size = sets*associativity*blockSize;

    // Check size, block size and associativity are 2^N
    assert((size & (size - 1)) == 0);
    assert((blockSize & (blockSize - 1)) == 0);
    assert((associativity & (associativity - 1)) == 0);
    assert(size >= blockSize);

    // Initialize cache configuration
    blockNum = size / (blockSize * associativity);
    indexMask = blockNum - 1;
    indexOffset = calcLog2(blockSize);
    tagOffset = calcLog2(blockNum) + indexOffset;

    debug("indexOffset %d", indexOffset);
    debug("indexMask 0x%x", indexMask);
    debug("tagOffset %d", tagOffset);

    hits = 0;
    misses = 0;
    evictions = 0;
  }

  void cache::evict(std::list<cacheLine>* lines,
      std::list<cacheLine>::iterator victim) 
  {
    debug("miss evict victim %lx", victim->addr);
    evictions++;
    lines->erase(victim);
  }

  bool cache::needEviction(const std::list<cacheLine>& lines, long addr) 
  {
    return (lines.size() < associativity ? false : true); 
  }

  void cache::allocateLine(std::list<cacheLine>& lines, long addr, int coreid) 
  {
    if (needEviction(lines, addr)) 
    {
      // Get victim.
      auto victim =lines.begin();
      assert(victim != lines.end());
      evict(&lines, victim);
    }
    // Allocate newline, with dirty bit off
    lines.push_back(cacheLine(addr, getTag(addr)));
  }

  bool cache::isHit(std::list<cacheLine>& lines, long addr,
      std::list<cacheLine>::iterator* pos_ptr) 
  {
    auto pos = find_if(lines.begin(), lines.end(),
        [addr, this](cacheLine l){return (l.tag == getTag(addr));});
    *pos_ptr = pos;
    if (pos == lines.end()) {
      return false;
    }
    return true;
  }

  void cache::cacheLogic(char operation, long address)
  {
    debug("address %lx type %d, index %d, tag %ld",
          address, operation, getIndex(address),
        getTag(address));

    // If there isn't a set, create it.
    auto& caSet = getLine(address);
    std::list<cacheLine>::iterator line;
    
    if (isHit(caSet, address, &line)) 
    {
      caSet.push_back(cacheLine(address, getTag(address)));
      caSet.erase(line);
      hits++;
    }
    else 
    {
      misses++;
      allocateLine(caSet, address, 0);
    }
  }
} // namespace cacheSim