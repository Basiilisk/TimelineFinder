#include "pointsview.h"

void RowBandTreeView::drawRow(QPainter* p, const QStyleOptionViewItem& opt, const QModelIndex& idx) const
{
    // Ask the model for a background brush (per-row, column 0)
    const QModelIndex row0 = idx.sibling(idx.row(), 0);
    const QVariant bgv = row0.data(Qt::BackgroundRole);
    if (bgv.canConvert<QBrush>()) {
        const QBrush brush = qvariant_cast<QBrush>(bgv);

        // Rectangle for entire row
        QRect band(2, opt.rect.y() + 1, viewport()->width() - 4, opt.rect.height() - 2);

        p->save();
        p->setRenderHint(QPainter::Antialiasing, true);
        p->setPen(Qt::NoPen);
        p->setBrush(brush);

        // Rounded corners (radius = 6 px for example)
        p->drawRoundedRect(band, 6, 6);
        p->restore();
    }

    // Draw normal text, arrows, etc.
    QTreeView::drawRow(p, opt, idx);
}
