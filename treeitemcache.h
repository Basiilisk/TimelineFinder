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

    void putFound(const QString& path);
    TreeItem* getRawFound(const QString& path) const;

    TreeItem* getRawSwither(const QString& path, bool all = true);

private:
    std::unordered_map<QString, std::unique_ptr<TreeItem>> cacheItems;
    std::unordered_map<QString, std::unique_ptr<TreeItem>> cacheFoundItems;
};
