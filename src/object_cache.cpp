#include "object_cache.hpp"

ObjectCache& ObjectCache::getInstance() {
    static ObjectCache instance;

    return instance;
}

void ObjectCache::clear() {
    mapping_.clear();
}
