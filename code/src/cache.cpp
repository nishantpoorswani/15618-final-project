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

void cache::evict(std::list<cacheLine>* lines,
    std::list<cacheLine>::iterator victim) {
  debug("miss evict victim %lx", victim->addr);
  evictions++;
  lines->erase(victim);
}

bool cache::needEviction(const std::list<cacheLine>& lines, long addr) {
  // if (find_if(lines.begin(), lines.end(),
  //     [addr, this](cacheLine l){
  //       return (getTag(addr) == l.tag);})
  //     != lines.end()) {
  //   assert(false);
  // } else {
    if (lines.size() < associativity) {
      return false;
    } else {
      return true;
    }
  //}
}

std::list<cache::cacheLine>::iterator cache::allocateLine(
    std::list<cacheLine>& lines, long addr, int coreid) 
{
  if (needEviction(lines, addr)) 
  {
    // Get victim.
    // The first one might still be locked due to reorder in MC
    auto victim =lines.begin();
    assert(victim != lines.end());
    evict(&lines, victim);
  }

  // Allocate newline, with lock bit on and dirty bit off
  lines.push_back(cacheLine(addr, getTag(addr)));
  auto last_element = lines.end();
  --last_element;
  return last_element;
}

bool cache::isHit(std::list<cacheLine>& lines, long addr,
    std::list<cacheLine>::iterator* pos_ptr) {
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
    caSet.push_back(cacheLine(address, getTag(address), false,
        line->dirty || (operation == 'S')));
    caSet.erase(line);
    hits++;
    //return true;
  }
  else 
  {
    misses++;
    // The dirty bit will be set if this is a write request and @L1
    //bool dirty = (operation == 'S');
    //auto newline = allocateLine(caSet, address, 0);
    allocateLine(caSet, address, 0);
    // if (newline == caSet.end()) {
    //   return false;
    // }
  }
  std::cout << "hits:" << hits << " misses:" << misses << " evictions" << evictions << "\n";
}

} // namespace cacheSim