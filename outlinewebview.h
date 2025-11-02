#pragma once
#include <QString>
#include <QVector>
#include <QWebEngineFindTextResult>
#include <QWebEnginePage>
#include <QWidget>

class QWebEngineView;

struct OutlineSection {
    QString id; // e.g. "1.", "1.1.", "1.2.3."
    QString title; // Optional title text
    QString body; // Section text
};

class OutlineWebView : public QWidget {
    Q_OBJECT
public:
    explicit OutlineWebView(QWidget* parent = nullptr);

    // Set sections directly (no file I/O)
    void setSections(const QVector<OutlineSection>& sections, bool openAll = false);

    // Expand/Collapse all
    void expandAll();
    void collapseAll();

    // Scroll to point by ID ("1.2.3.")
    void scrollToPoint(const QString& marker,
        bool openAncestors = true,
        bool smooth = true,
        bool highlight = true);

    // Search text
    void findText(const QString& query,
        bool forward = true,
        bool caseSensitive = false,
        bool wrapAround = true);

private:
    QString buildHtml(const QVector<OutlineSection>& sections, bool openAll) const;
    static QString jsQuote(const QString& s);
    QWebEngineView* m_view = nullptr;
};
