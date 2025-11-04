#include "pointsmodel.h"

#include <QBrush>

PointsModel::PointsModel(QObject* parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex PointsModel::rootIndex()
{
    return createIndex(0, 0, root);
}

// PointsModel::~PointsModel()
// {
//     delete root;
// }

QModelIndex PointsModel::index(int row, int column, const QModelIndex& parent) const
{
    if (row < 0 || column < 0 || column >= columnCount(parent))
        return {};

    TreeItem* p = itemFromIndex(parent);
    TreeItem* child = p->child(row);

    if (!child)
        return {};

    return createIndex(row, column, child); // internalPointer = TreeItem*
}

QModelIndex PointsModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return {};

    auto* item = itemFromIndex(child);
    TreeItem* p = item->parent();

    if (!p || p == root)
        return {};

    return createIndex(p->row(), 0, p);
}

int PointsModel::rowCount(const QModelIndex& parent) const
{
    TreeItem* parentItem = itemFromIndex(parent);
    if (!parentItem)
        parentItem = root;

    return parentItem ? parentItem->childCount() : 0;
}

int PointsModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 2;
}

QVariant PointsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return {};

    auto* item = itemFromIndex(index);

    qDebug() << ">item:" << item << " index:" << index;

    QString name;
    bool hasName = item->hasName(name);

    if (role == Qt::BackgroundRole) {
        if (hasName)
            return QBrush(QColor("#e0f7fa"));
    }

    // --- Text color example ---
    if (role == Qt::ForegroundRole) {
        if (hasName)
            return QBrush(QColor(Qt::black));
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        if (index.column() == 1) {
            if (hasName)
                return name;
            return " -#- ";

        } else
            return item->data();
    }
    return {};
}

Qt::ItemFlags PointsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant PointsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
        case 0:
            return "Point ID"; // column 0
        case 1:
            return "Name"; // column 1
        default:
            return {};
        }
    }
    return {};
}

bool PointsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    auto* item = itemFromIndex(index);
    if (!item->setData(value))
        return false;

    emit dataChanged(index, index, { Qt::DisplayRole, Qt::EditRole });
    return true;
}

TreeItem* PointsModel::itemFromIndex(const QModelIndex& idx) const
{
    return idx.isValid() ? static_cast<TreeItem*>(idx.internalPointer()) : root;
}

TreeItem* PointsModel::getRoot()
{
    return root;
}

TreeItem* PointsModel::find(TreeItem* parent, QString val)
{
    TreeItem* result = nullptr;
    if (parent->data().toString() == val)
        return parent;

    for (int i = 0; i < parent->childCount(); ++i) {
        auto p = parent->child(i);
        result = find(p, val);
        if (result != nullptr)
            break;
    }

    return result;
}

void PointsModel::printTree(TreeItem* parent)
{
    for (int i = 0; i < parent->childCount(); ++i) {
        auto p = parent->child(i);
        printTree(p);
    }

    qDebug() << "<-" << parent->data().toString();
}
