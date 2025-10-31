#include "parseandread.h"

#include <QFile>
#include <QIODevice>

ParseAndRead::ParseAndRead()
{
}

int ParseAndRead::parse(QString& fileName)
{
    QString path = "C:/Users/svyat/Desktop/Syava_stroyova/convTest/DocToTxt/" + fileName;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Error: can't open file:" << fileName;
        return -1;
    }

    QString rawText;
    QTextStream ts(&f);
    ts.setEncoding(QStringConverter::Utf8);
    while (!ts.atEnd()) {
        rawText = ts.readAll();
    }

    QVector<QString> allTexts;
    QVector<Section> parts = splitByPoints(rawText, &allTexts);

    filePoints.insert(fileName, parts);

    return 0;
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

QHash<QString, QVector<Section>>& ParseAndRead::filePoint()
{
    return filePoints;
}

void ParseAndRead::clearAll()
{
    filePoints.clear();
}
