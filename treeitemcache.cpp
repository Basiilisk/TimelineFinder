#include "treeitemcache.h"

void TreeItemCache::put(const QString& path)
{
    cacheItems.emplace(path, std::make_unique<TreeItem>("root")); // in-place, move
}

TreeItem* TreeItemCache::getRaw(const QString& path) const
{
    auto it = cacheItems.find(path);
    return (it == cacheItems.end()) ? nullptr : it->second.get();
}
