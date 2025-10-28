#include "mainwindow.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QLabel>
#include <QLayout>
#include <QListView>
#include <QMenuBar>
#include <QProcess>
#include <QPushButton>
#include <QSplitter>
#include <QStandardItemModel>
#include <QTableView>
#include <QTextEdit>
#include <QTreeView>
#include <QUrl>

#include "filemodel.h"
#include "parseandread.h"
#include "pointsmodel.h"
#include "textmodel.h"
#include "treeitem.h"

void updateHLT(QTextCursor* cursor, int rowI, QTextCharFormat* fmt);

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // QProcess::startDetached("C:\\Windows\\explorer.exe", {});
    /*
    QString filePath = "C:/Users/svyat/Desktop/Syava_stroyova/Ф12-ГОЗА В А.docx"; // Use forward slashes
    //QDesktopServices::openUrl(QUrl(filePath));
    QUrl url = QUrl::fromLocalFile(QFileInfo(filePath).absoluteFilePath());
    QDesktopServices::openUrl(url);
    */

    QWidget* mainW = new QWidget;
    w.setWindowTitle("TimelineFinder");
    w.setCentralWidget(mainW);

    QMenuBar* mainMenu = new QMenuBar();
    QLabel* infoLabel = new QLabel("Клікни по клітинці 👇");

    QMenu* fileMenu1 = new QMenu("11Файл11");
    QMenu* helpMenu1 = new QMenu("11Довідка11");
    QMenu* helpMenu2 = new QMenu("11Довідка22");
    QMenu* helpMenu3 = new QMenu("11Довідка33");

    fileMenu1->addMenu(helpMenu1);
    helpMenu1->addMenu(helpMenu2);
    helpMenu1->addMenu(helpMenu3);

    QMenu* fileMenu = new QMenu("Файл");
    QMenu* helpMenu = new QMenu("Довідка");
    mainMenu->addMenu(fileMenu);
    mainMenu->addMenu(helpMenu);

    mainMenu->addMenu(fileMenu1);

    helpMenu->addAction("AboutQt", &a, SLOT(aboutQt()));
    fileMenu->addAction("&Exit", &w, SLOT(close()), Qt::CTRL | Qt::Key_Q);

    QSplitter* splitter = new QSplitter();
    QPushButton* butt1 = new QPushButton("Parsing");
    QPushButton* butt2 = new QPushButton("Update");
    butt2->setCheckable(true);

    QVBoxLayout* vLayout = new QVBoxLayout();
    QHBoxLayout* hLayout = new QHBoxLayout();

    mainW->setLayout(vLayout);

    FileModel* fileModel = new FileModel(mainW);
    fileModel->setHorizontalHeaderItem(0, new QStandardItem("Name File"));

    PointsModel* pointsModel = new PointsModel(mainW);
    // pointsModel->setHorizontalHeaderItem(0, new QStandardItem("Points"));

    QTableView* fileView = new QTableView();
    fileView->setModel(fileModel);
    fileView->setAlternatingRowColors(true);

    QTreeView* pointsView = new QTreeView();
    pointsView->setModel(pointsModel);
    // pointsView->setRootIsDecorated(true);
    pointsView->setAlternatingRowColors(true);

    QTextEdit* text = new QTextEdit(mainW);
    auto* splliter = new QSplitter;
    splliter->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    splliter->addWidget(fileView);
    splliter->addWidget(pointsView);
    splliter->addWidget(text);

    vLayout->addWidget(mainMenu);
    vLayout->addLayout(hLayout);

    hLayout->addWidget(splliter);
    hLayout->addWidget(butt1);
    vLayout->addWidget(butt2);
    vLayout->addWidget(infoLabel);

    w.show();

    //===========================================================================================
    QString fileName = "00____myShityTestFile____00.txt";
    QString path = "C:/Users/svyat/Desktop/Syava_stroyova/converted/DocToTxt/" + fileName;

    QString rawText;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return -1;

    QTextStream ts(&f);
    ts.setEncoding(QStringConverter::Utf8);
    while (!ts.atEnd()) {
        rawText = ts.readAll();
    }

    //===========================================================================================
    ParseAndRead parts;

    parts.parse(fileName, rawText);
    auto sections = parts.pointData(fileName);

    // Example: print IDs and first 40 chars of each chunk
    for (const auto& s : sections) {
        qDebug().noquote() << s.id << "marker= " << s.marker
                           << " title=" << s.title
                           << "   preview=" << s.text.left(400).replace('\n', ' ');
    }

    //===========================================================================================
    QString outText;

    for (const auto& s : sections) {
        outText += s.marker + " ID: " + s.id + "\nTITLE:" + s.title + "\nTEXT:\n" + s.text + "\n";
    }

    QString outPath = QDir::currentPath() + "/OUT.txt";
    QFile out(outPath);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "Cannot write:" << outPath;
        return -1;
    }
    QTextStream tsOut(&out);
    tsOut.setEncoding(QStringConverter::Utf8);
    tsOut << outText;
    tsOut.flush();
    out.close();

    auto* parentItem = pointsModel->getRoot();

    for (auto fileName : parts.filesName()) {
        fileModel->appendRow(new QStandardItem(fileName));

        for (auto points : parts.pointData(fileName)) {
            int dotCount = points.marker.count('.');
            int depth = dotCount - 1; // subtract the final dot
            QString indent(depth * 4, ' '); // 4 spaces per level

            //qDebug() << "INFO:" << points.marker << dotCount << depth;
            QString val = points.marker;
            auto it = pointsModel->find(parentItem, val);
            if (it == nullptr) {

                QString s = val;
                bool find = false;
                while (s.count('.') > 1) {
                    s = shortenPath(s);
                    auto itS = pointsModel->find(parentItem, s);
                    if (itS != nullptr) {
                        qDebug() << "Parent" << itS->data().toString() << " for " << val;
                        itS->appendChild(val);
                        find = true;
                        break;
                    }
                }

                if (!find)
                    parentItem->appendChild(val);

            } else {
                qDebug() << "\n\nError val it's already exist:" << val;
                return -1;
            }

            // qDebug() << points.marker;
        }
    }

    FileModel::connect(pointsView, &QTreeView::clicked, &w, [&](const QModelIndex& index) {
        QString t = index.data().toString();
        //rowI = index.row();
        infoLabel->setText(QString("Ви клікнули: [%1, %2] = %3")
                               .arg(index.row())
                               .arg(index.column())
                               .arg(t));
        //updateHLT(cursor, rowI, fmt);
        QString poitnText = parts.poinText(fileName,t);
        text->setText(poitnText);
    });

    return a.exec();
}
