#pragma once

#include <QString>
#include <QVariant>
#include <QVector>

class TreeItem {

public:
    explicit TreeItem(QString val, QString name = "", TreeItem* parent = nullptr);

    TreeItem(const TreeItem&) = delete;
    TreeItem& operator=(const TreeItem&) = delete;
    TreeItem(TreeItem&&) noexcept = default;
    TreeItem& operator=(TreeItem&&) noexcept = default;

    // --- читання структури ---
    TreeItem* parent() const;
    int childCount() const;
    TreeItem* child(int row) const;
    QVector<TreeItem*> children();
    int row() const; // індекс серед братів (для QAbstractItemModel::parent)

    QVariant data() const;
    bool setData(const QVariant& v);
    bool hasName(QString& n) const;

    TreeItem* appendChild(QString val, QString name = "");

    bool removeChild(int row);
    // Забрати дитину (передати власність нагору/назовні)
    // Корисно для "переміщення" вузла в інше місце дерева.
    std::unique_ptr<TreeItem> takeChild(int row);

    // Очистити всіх дітей
    void clear();

private:
    QString name;
    QString val;
    std::vector<std::unique_ptr<TreeItem>> m_children;
    TreeItem* m_parent = nullptr;
};
