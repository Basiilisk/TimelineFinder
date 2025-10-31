#pragma once

#include "treeitem.h"

class TreeItemCache {
public:
    TreeItemCache() = default;
    ~TreeItemCache() = default;

    TreeItemCache(const TreeItemCache&) = delete;
    TreeItemCache& operator=(const TreeItemCache&) = delete;

    void put(const QString& path);
    TreeItem* getRaw(const QString& path) const;

private:
    std::unordered_map<QString, std::unique_ptr<TreeItem>> cacheItems;
};
