#pragma once

#include <QStandardItemModel>

class FileModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit FileModel(QObject *parent = nullptr);
};
