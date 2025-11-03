#pragma once

#include <QPainter>
#include <QTreeView>

class RowBandTreeView : public QTreeView {
    Q_OBJECT
public:
    using QTreeView::QTreeView;

protected:
    void drawRow(QPainter* p, const QStyleOptionViewItem& opt,
        const QModelIndex& idx) const override;
};
