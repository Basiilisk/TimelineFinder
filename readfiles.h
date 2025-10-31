#pragma once

#include <QDateTime>
#include <QFileInfo>
#include <QRegularExpression>
#include <QString>

// -------------------- Regexes --------------------
// Cyrillic word tokens with apostrophe/hyphen
static QRegularExpression tokenRe(R"((\p{Cyrillic}[\p{Cyrillic}'’\-]+))",
    QRegularExpression::CaseInsensitiveOption);
// Outline lines: "1.", "1.1.", "1 . 1 . 4", "26)" (title optional)
static QRegularExpression outlineRe(
    R"(^(?:\s*)(\d{1,2}(?:\.\d{1,2})*)\.(?:\s*(.*))?$)",
    QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);
// Date + order in header (first 500 chars)
static QRegularExpression dateRe(R"(\b(\d{2})\.(\d{2})\.(\d{4})\b)",
    QRegularExpression::CaseInsensitiveOption);
static QRegularExpression ordRe(R"(№\s*(\d+))",
    QRegularExpression::CaseInsensitiveOption);

struct HeaderInfo {
    QString orderNo;
    QString date;
    QDate dt;
};

struct NameOccur {
    int start = 0, end = 0;
    QString fullname;
};

struct OutlineLine {
    int start = 0, end = 0;
    QString line;
};

struct FileResult {
    QString filePath;
    QString fileName;
    HeaderInfo header;
    QVector<QPair<QString, QPair<QString, QString>>>
        records; // fullname, (above, below)
};

class ReadFiles {
public:
    ReadFiles();

    void collectFiles();
    FileResult processFile(const QFileInfo& fi,
        const QString& lastStem,
        const QString& firstStem,
        const QString& patrStem);

    HeaderInfo extractHeaderInfo(const QString& text,
        int headerChars = 500);
    QVector<OutlineLine> extractOutlines(const QString& text);
    QVector<NameOccur> findAllNameOccurrences(const QString& text,
        const QString& lastStem,
        const QString& firstStem,
        const QString& patrStem);

    QList<FileResult> &result();

private:
    int allFiles = 0;
    int nameFiles = 0;
    QList<FileResult> results;
};
