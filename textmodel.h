#pragma once

#include <QStandardItemModel>

class TextModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit TextModel(QObject *parent = nullptr);
};
