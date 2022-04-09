#include "../include/cache.h"

#define DEBUG_CACHE

#ifndef DEBUG_CACHE
#define debug(...)
#else
#define debug(...) do { \
          printf("\033[36m[DEBUG] %s ", __FUNCTION__); \
          printf(__VA_ARGS__); \
          printf("\033[0m\n"); \
      } while (0)
#endif

cache::cache(int sets, int associativity, int blockSize):
    sets(sets), associativity(associativity),
    blockSize(blockSize){

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
    dirty_bytes = 0;
    dirty_evictions = 0;
}

bool Cache::needEviction(const std::list<Line>& lines, long addr) {
  if (find_if(lines.begin(), lines.end(),
      [addr, this](Line l){
        return (get_tag(addr) == l.tag);})
      != lines.end()) {
    // Due to MSHR, the program can't reach here. Just for checking
    assert(false);
  } else {
    if (lines.size() < associativity) {
      return false;
    } else {
      return true;
    }
  }
}

std::list<cache::cacheLine>::iterator cache::allocateLine(
    std::list<cacheLine>& lines, long addr, int coreid) {
   if (needEviction(lines, addr)) {
      // Get victim.
      // The first one might still be locked due to reorder in MC
      auto victim = find(lines.begin(), lines.end());
      if (victim == lines.end()) {
        return victim;  // doesn't exist a line that's already unlocked in each level
      }
      assert(victim != lines.end());
      evict(&lines, victim);
    }

    // Allocate newline, with lock bit on and dirty bit off
    lines.push_back(Line(addr, get_tag(addr)));
    auto last_element = lines.end();
    --last_element;
    return last_element;
  }
}

void cache::cacheLogic(char operation, cache* cacheCore, long address){

    debug("address %lx type %d, index %d, tag %ld",
         address, operation, get_index(address),
        get_tag(address));

    // If there isn't a set, create it.
    auto& caSet = getLine(address);
    std::list<cacheLine>::iterator line;

    if (cacheCore->isHit(caSet, address, &line)) {
      caSet.push_back(cacheLine(address, getTag(address), false,
          line->dirty || (operation == "S")));
      caSet.erase(line);
      cacheCore->hits++;
      return true;
    }
    else {
      debug("miss @level %d", int(level));
      cacheCore->misses++;
   
      // The dirty bit will be set if this is a write request and @L1
      bool dirty = (operation == "S");
   
      auto newline = allocateLine(caSet, address, 0);
      if (newline == caSet.end()) {
        return false;
      }
    }
}