#include "readfiles.h"

#include <QDirIterator>
#include <QRegularExpression>

QString rootDir = R"(C:\Users\svyat\Desktop\Syava_stroyova\converted\DocToTxt)";

// QString lastStem = ("Гарап");
// QString firstStem = ("");
// QString patrStem = ("");

ReadFiles::ReadFiles()
{
}

void ReadFiles::collectFiles(const QString& lastStem, const QString& firstStem, const QString& patrStem)
{
    // Collect files
    QList<QFileInfo> files;
    QDirIterator it(rootDir, QStringList() << "*.txt", QDir::Files,
        QDirIterator::Subdirectories);
    while (it.hasNext()) {
        files << QFileInfo(it.next());
        ++allFiles;
    }

    // Process
    results.reserve(files.size());
    for (const auto& fi : std::as_const(files)) {
        auto r = processFile(fi, lastStem, firstStem, patrStem);
        if (!r.records.isEmpty()) {
            results.push_back(std::move(r));
            ++nameFiles;
        }
    }

    // Sort by header date then filename
    std::sort(results.begin(), results.end(),
        [](const FileResult& a, const FileResult& b) {
            const bool aHas = a.header.dt.isValid();
            const bool bHas = b.header.dt.isValid();
            if (aHas != bHas)
                return aHas && !bHas;
            if (aHas && bHas && a.header.dt != b.header.dt)
                return a.header.dt < b.header.dt;
            return a.fileName < b.fileName;
        });
}

FileResult ReadFiles::processFile(const QFileInfo& fi, const QString& lastStem, const QString& firstStem, const QString& patrStem)
{
    FileResult R;
    R.fileName = fi.fileName();
    QFile f(fi.absoluteFilePath());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};
    QTextStream ts(&f);
    ts.setEncoding(QStringConverter::Utf8);
    QString text = ts.readAll();

    R.header = extractHeaderInfo(text, 500);
    const auto outlines = extractOutlines(text);
    const auto occs = findAllNameOccurrences(text, lastStem, firstStem, patrStem);

    for (const auto& occ : occs) {
        QString above, below;
        for (const auto& ol : outlines) {
            if (ol.start < occ.start)
                above = ol.line;
            else if (ol.start > occ.start) {
                below = ol.line;
                break;
            }
        }
        R.records.push_back({ occ.fullname, { above, below } });
    }
    return R;
}

HeaderInfo ReadFiles::extractHeaderInfo(const QString& text, int headerChars)
{
    HeaderInfo h;
    const QString head = text.left(headerChars);

    // №
    if (auto m = ordRe.match(head); m.hasMatch())
        h.orderNo = m.captured(1);

    // closest date to first № (if present)
    QList<QPair<int, QString>> cands;
    int ordPos = -1;
    if (!h.orderNo.isEmpty()) {
        auto om = ordRe.match(head);
        if (om.hasMatch())
            ordPos = om.capturedStart();
    }
    auto it = dateRe.globalMatch(head);
    while (it.hasNext()) {
        auto m = it.next();
        const QString d = m.captured(0);
        const int dist = (ordPos >= 0) ? std::abs(m.capturedStart() - ordPos) : cands.size();
        cands.push_back({ dist, d });
    }
    if (!cands.isEmpty()) {
        std::sort(cands.begin(), cands.end(),
            [](auto& a, auto& b) { return a.first < b.first; });
        h.date = cands.first().second;
        h.dt = QDate::fromString(h.date, "dd.MM.yyyy");
    }
    return h;
}

QVector<OutlineLine> ReadFiles::extractOutlines(const QString& text)
{
    QVector<OutlineLine> res;
    auto it = outlineRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        QString num = m.captured(1);
        // num.replace(QRegularExpression(R"(\s*\.\s*)"), "."); // "1 . 2" -> "1.2"
        //  skip if it looks like a date (dd.mm.yyyy)
        static QRegularExpression dateLike(R"(^\d{1,2}\.\d{1,2}\.\d{4}$)");
        // static QRegularExpression dateLike(R"(^\d{1,2}\.\d{1,2}\.\d{4}$)");
        if (dateLike.match(num).hasMatch()) {
            continue; // ignore this match
        }
        const QString title = m.captured(2).trimmed();
        const QString line = (num.isEmpty() ? title : (num + ". " + title)).trimmed();
        res.push_back({ static_cast<int>(m.capturedStart()),
            static_cast<int>(m.capturedEnd()), line });
    }
    return res;
}

QVector<NameOccur> ReadFiles::findAllNameOccurrences(const QString& text, const QString& lastStem, const QString& firstStem, const QString& patrStem)
{
    // Case-insensitive by comparing casefolded strings
    const QString Ls = lastStem.toCaseFolded();
    const QString Fs = firstStem.toCaseFolded();
    const QString Ps = patrStem.toCaseFolded();

    // Tokenize Cyrillic words
    QVector<QPair<QString, QPair<int, int>>> tokens;
    auto it = tokenRe.globalMatch(text);
    while (it.hasNext()) {
        auto m = it.next();
        tokens.push_back({ m.captured(1), { m.capturedStart(), m.capturedEnd() } });
    }

    QVector<NameOccur> out;
    for (int i = 0; i + 2 < tokens.size(); ++i) {
        const QString& w1 = tokens[i].first;
        const QString& w2 = tokens[i + 1].first;
        const QString& w3 = tokens[i + 2].first;
        const bool ok1 = Ls.isEmpty() || w1.toCaseFolded().startsWith(Ls);
        const bool ok2 = Fs.isEmpty() || w2.toCaseFolded().startsWith(Fs);
        const bool ok3 = Ps.isEmpty() || w3.toCaseFolded().startsWith(Ps);
        if (ok1 && ok2 && ok3) {
            out.push_back({ tokens[i].second.first, tokens[i + 2].second.second,
                w1 + " " + w2 + " " + w3 });
        }
    }
    return out;
}

QList<FileResult>& ReadFiles::result()
{
    return results;
}
