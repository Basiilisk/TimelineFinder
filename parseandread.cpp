#include "parseandread.h"

ParseAndRead::ParseAndRead()
{
}

void ParseAndRead::parse(QString& fileName, QString& rawText)
{
    QVector<QString> allTexts;
    QVector<Section> parts = splitByPoints(rawText, &allTexts);

    filePoints.insert(fileName, parts);
}

QVector<Section> ParseAndRead::pointData(QString& key)
{
    return filePoints.find(key)->toVector();
}

QVector<QString> ParseAndRead::filesName()
{
    return filePoints.keys();
}

QMap<QString, QVector<Section>>& ParseAndRead::filePoint()
{
    return filePoints;
}
