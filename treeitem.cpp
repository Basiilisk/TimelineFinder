#include "treeitem.h"

TreeItem::TreeItem(QString v, QString name, TreeItem* m_parent)
    : val(std::move(v))
    , name(name)
    , m_parent(m_parent)
{
}

TreeItem* TreeItem::parent() const
{
    return m_parent;
}

int TreeItem::childCount() const
{
    return m_children.size();
}

TreeItem* TreeItem::child(int row) const
{
    if (row < 0 || row >= m_children.size())
        return nullptr;

    return m_children.at(row).get();
}

QVector<TreeItem*> TreeItem::children()
{
    QVector<TreeItem*> c;
    for (int i = 0; i < childCount(); ++i) {
        c.push_back(child(i));
    }

    return c;
}

int TreeItem::row() const
{
    if (!m_parent)
        return -1;

    const int n = m_parent->childCount();
    for (int i = 0; i < n; ++i) {
        if (m_parent->child(i) == this)
            return i;
    }
    return -1;
}

QVariant TreeItem::data() const
{
    return val;
}

bool TreeItem::setData(const QVariant& v)
{
    val = v.toString();
    return true;
}

bool TreeItem::hasName(QString& n) const
{
    n = name;
    return !name.isEmpty();
}

TreeItem* TreeItem::appendChild(QString val, QString name)
{
    m_children.push_back(std::make_unique<TreeItem>(std::move(val), name, this));
    return m_children.back().get();
}

bool TreeItem::removeChild(int row)
{
    if (row < 0 || row >= m_children.size())
        return false;

    m_children.erase(m_children.begin() + row);
    return true;
}

std::unique_ptr<TreeItem> TreeItem::takeChild(int row)
{
    if (row < 0 || row >= m_children.size())
        return {};

    // забираємо власність на елемент
    std::unique_ptr<TreeItem> out = std::move(m_children[row]);

    // стираємо позицію row (ітераторний erase)
    m_children.erase(m_children.begin() + row);

    if (out)
        out->m_parent = nullptr;
    return out;
}

void TreeItem::clear()
{
    m_children.clear();
}
