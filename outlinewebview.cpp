#include "OutlineWebView.h"
#include <QVBoxLayout>
#include <QWebEngineView>

OutlineWebView::OutlineWebView(QWidget* parent)
    : QWidget(parent)
    , m_view(new QWebEngineView(this))
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(m_view);
    m_view->setContextMenuPolicy(Qt::NoContextMenu);
}

void OutlineWebView::setSections(const QVector<OutlineSection>& sections, bool openAll)
{
    m_view->setHtml(buildHtml(sections, openAll));
}

void OutlineWebView::expandAll()
{
    m_view->page()->runJavaScript("if(window.expandAll)expandAll();");
}

void OutlineWebView::collapseAll()
{
    m_view->page()->runJavaScript("if(window.collapseAll)collapseAll();");
}

void OutlineWebView::scrollToPoint(const QString& marker,
    bool openAncestors,
    bool smooth,
    bool highlight)
{
    const QString js = QString(R"JS(
(function(){
 const id=%1;
 const d=document.querySelector(`details[data-id="${id}"]`);
 if(!d)return false;
 if(%2){
   let cur=d;
   while(cur){
     if(cur.tagName==='DETAILS')cur.open=true;
     cur=cur.parentElement?cur.parentElement.closest('details'):null;
   }
 }
 const sum=d.querySelector('summary')||d;
 sum.scrollIntoView({behavior:%3?'smooth':'auto',block:'center'});
 if(%4){
   document.querySelectorAll('.hl').forEach(e=>e.classList.remove('hl'));
   sum.classList.add('hl');
   setTimeout(()=>sum.classList.remove('hl'),1200);
 }
 return true;
})();
)JS")
        .arg(jsQuote(marker))
        .arg(openAncestors ? "true" : "false")
        .arg(smooth ? "true" : "false")
        .arg(highlight ? "true" : "false");
    m_view->page()->runJavaScript(js);
}

void OutlineWebView::findText(const QString& query,
    bool forward,
    bool caseSensitive,
    bool wrapAround)
{
    QWebEnginePage::FindFlags flags;
    if (!forward)
        flags |= QWebEnginePage::FindBackward;
    if (caseSensitive)
        flags |= QWebEnginePage::FindCaseSensitively;

    m_view->page()->findText(query, flags,
        [=](const QWebEngineFindTextResult& res) {
            if (!wrapAround || query.isEmpty() || res.numberOfMatches() > 0)
                return;

            const char* jsStart = "var r=document.createRange();r.selectNodeContents(document.body);"
                                  "var s=window.getSelection();s.removeAllRanges();s.addRange(r);s.collapseToStart();";
            const char* jsEnd = "var r=document.createRange();r.selectNodeContents(document.body);"
                                "var s=window.getSelection();s.removeAllRanges();s.addRange(r);s.collapseToEnd();";

            m_view->page()->runJavaScript(forward ? jsStart : jsEnd, [=](const QVariant&) {
                m_view->page()->findText(query, flags);
            });
        });
}

QString OutlineWebView::buildHtml(const QVector<OutlineSection>& sections, bool openAll) const
{
    const QString css = R"CSS(
:root { color-scheme: light dark; }
body { font-family: Segoe UI, system-ui, sans-serif; font-size: 14px;
       line-height: 1.4; padding: 12px 18px; margin:0; background:none; color:inherit;}
details { margin: 3px 0; padding: 0; border: none; }
summary { cursor: pointer; list-style: none; font-weight: 500; padding: 0; margin: 0; }
summary::-webkit-details-marker { display: none; }
summary::before { content: "▸"; display:inline-block; width:1em; opacity:0.7; }
details[open]>summary::before { content: "▾"; }
.content { white-space: pre-wrap; padding-left: 1.5em; margin-bottom:4px; }
.hl { animation: flash 1.2s ease-in-out 1; background: rgba(255,230,0,0.45); }
@keyframes flash { 0%{background:rgba(255,230,0,0.9);} 100%{background:transparent;} }
)CSS";

    QString html;
    html += "<!doctype html><html><head><meta charset='utf-8'><style>" + css + "</style></head><body>\n";

    for (const auto& s : sections) {
        QString anchor = "sec-" + s.id;
        anchor.replace('.', '-');
        while (anchor.endsWith('-'))
            anchor.chop(1);
        const QString openAttr = openAll ? " open" : "";
        QString title = s.id.toHtmlEscaped();
        if (!s.title.isEmpty())
            title += " " + s.title.toHtmlEscaped();
        html += QString("<details%1 data-id='%2' id='%3'><summary>%4</summary><div class='content'>%5</div></details>\n")
                    .arg(openAttr,
                        s.id.toHtmlEscaped(),
                        anchor.toHtmlEscaped(),
                        title,
                        s.body.toHtmlEscaped());
    }

    html += R"(
                <script>
                function expandAll(){document.querySelectorAll('details').forEach(d=>d.open=true);}
                function collapseAll(){document.querySelectorAll('details').forEach(d=>d.open=false);}
                </script>
                </body></html>
            )";
    return html;
}

QString OutlineWebView::jsQuote(const QString& s)
{
    QString escaped = s;
    escaped.replace("\\", "\\\\");
    escaped.replace("\"", "\\\"");
    escaped.replace("\n", "\\n");
    escaped.replace("\r", "\\r");
    escaped.replace("\t", "\\t");
    return "\"" + escaped + "\"";
}
