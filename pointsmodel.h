#pragma once

#include <QAbstractItemModel>
#include <QStringLiteral>

#include "treeitem.h"

class PointsModel : public QAbstractItemModel {
    Q_OBJECT
public:
    explicit PointsModel(QObject* parent = nullptr);
    ~PointsModel() override = default;
    //~PointsModel() override;

    void setBorrowedRoot(TreeItem* currentRoot)
    {
        beginResetModel();
        root = currentRoot;
        qDebug() << currentRoot;
        endResetModel();
    }

    // --- must-have ---
    QModelIndex rootIndex();
    QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;

    // --- usual extras ---
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    // helper
    TreeItem* itemFromIndex(const QModelIndex& idx) const;
    TreeItem* getRoot();

    TreeItem* find(TreeItem* parent, QString val);
    void printTree(TreeItem* parent);

private:
    TreeItem* root = nullptr;
};
