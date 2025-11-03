#pragma once

#include <QRegularExpression>
#include <QString>

struct Section {
    QString id; // e.g. "P1_2_3"
    QString marker; // e.g. "1.2.3."
    QString title;
    int start = 0; // content start (after marker)
    int end = 0; // content end (before next marker or end of text)
    QString text; // extracted text between this marker and the next
    QString name;

    bool operator==(const QString& sec) const
    {
        return sec == marker;
    }
};

// Up to three segments, each 1–3 digits, separated by dots, optional trailing dot.
// No letters/digits attached directly before/after the marker.
static QRegularExpression kPointRe(
    // R"(^(?:\s*)(\d{1,2}(?:\.\d{1,2})*)\.(?:(.*))?$)",
    R"(^\s*(\d{1,3}(?:\.\d{1,3}){0,2})\.(?!\d)\s*(.*)$)",
    QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);

// Split text into sections between point markers.
// allTextsOut will contain just the raw chunk texts if you want a simple vector.
static QVector<Section> splitByPoints(const QString& lastStem, const QString& firstStem, const QString& patrStem, const QString& text, QVector<QString>* allTextsOut = nullptr)
{
    QVector<Section> sections;
    QVector<QPair<int, QRegularExpressionMatch>> hits;

    // 1) Collect all markers with their positions
    auto it = kPointRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        hits.push_back({ m.capturedStart(1), m }); // anchor on group(1) start
    }
    if (hits.isEmpty()) {
        if (allTextsOut)
            allTextsOut->clear();
        return sections;
    }

    for (auto i : hits) {
        qDebug() << "DATA in hits " << i.second.captured(1);
    }

    // 2) Build sections between markers
    sections.reserve(hits.size());
    for (int i = 0; i < hits.size(); ++i) {
        const auto& m = hits[i].second;
        const QString markerCore = m.captured(1); // e.g. "1.2.3"
        const int markerEnd = m.capturedEnd(); // end of the whole match (incl. trailing dot if present)
        const int nextStart = (i + 1 < hits.size()) ? hits[i + 1].second.capturedStart() : text.size();

        Section s;
        s.title = m.captured(2);
        s.marker = markerCore + '.';
        s.id = QStringLiteral("P") + markerCore;
        s.id.replace('.', '_');

        // Content = between end of current marker and start of next marker
        s.start = markerEnd;
        s.end = qMax(markerEnd, nextStart);
        s.text = text.mid(s.start, s.end - s.start).trimmed();

        QVector<QPair<QString, QPair<int, int>>> tokens;

        auto it = QRegularExpression(R"((\p{Cyrillic}[\p{Cyrillic}'’\-]+))",
            QRegularExpression::CaseInsensitiveOption)
                      .globalMatch(s.title + s.text);
        while (it.hasNext()) {
            auto m = it.next();
            tokens.push_back({ m.captured(1), { m.capturedStart(), m.capturedEnd() } });
        }

        const QString Ls = lastStem.toCaseFolded();
        const QString Fs = firstStem.toCaseFolded();
        const QString Ps = patrStem.toCaseFolded();

        for (int i = 0; i + 2 < tokens.size(); ++i) {
            const QString& w1 = tokens[i].first;
            const QString& w2 = tokens[i + 1].first;
            const QString& w3 = tokens[i + 2].first;
            const bool ok1 = Ls.isEmpty() || w1.toCaseFolded().startsWith(Ls);
            const bool ok2 = Fs.isEmpty() || w2.toCaseFolded().startsWith(Fs);
            const bool ok3 = Ps.isEmpty() || w3.toCaseFolded().startsWith(Ps);
            if (ok1 && ok2 && ok3) {
                s.name = w1 + " " + w2 + " " + w3;
                qDebug() << "|!!!| NAME: " << s.name;
            }
        }

        sections.push_back(std::move(s));
    }

    if (allTextsOut) {
        allTextsOut->clear();
        allTextsOut->reserve(sections.size());
        for (const auto& s : sections)
            allTextsOut->push_back(s.text);
    }

    return sections;
}

static QString shortenPath(const QString& input)
{
    QString s = input;
    if (s.endsWith('.'))
        s.chop(1);
    QStringList parts = s.split('.', Qt::SkipEmptyParts);
    if (parts.size() > 1)
        parts.removeLast();
    return parts.join('.') + ".";
}

class ParseAndRead {
public:
    ParseAndRead();

    int parse(const QString& lastStem, const QString& firstStem, const QString& patrStem, QString& fileName);

    QString poinText(QString& key, QString val);
    QVector<Section> pointData(QString& key);
    QVector<QString> filesName();
    QHash<QString, QVector<Section>>& filePoint();
    void clearAll();

private:
    QHash<QString, QVector<Section>> filePoints;
};
