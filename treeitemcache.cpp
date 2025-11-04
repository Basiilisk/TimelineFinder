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

void TreeItemCache::putFound(const QString& path)
{
    cacheFoundItems.emplace(path, std::make_unique<TreeItem>("found")); // in-place, move
}

TreeItem* TreeItemCache::getRawFound(const QString& path) const
{
    auto it = cacheFoundItems.find(path);
    return (it == cacheFoundItems.end()) ? nullptr : it->second.get();
}

TreeItem* TreeItemCache::getRawSwither(const QString& path, bool all)
{
    if (all)
        return getRaw(path);

    return getRawFound(path);
}
