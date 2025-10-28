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

QString ParseAndRead::poinText(QString& key, QString val)
{
    auto points = pointData(key);
    int res = points.indexOf(val);
    QString text("Error");
    if (res != -1)
        text = points.at(res).marker + " " + points.at(res).title + points.at(res).text;

    return text;
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
