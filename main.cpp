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
#include "readfiles.h"
#include "textmodel.h"
#include "treeitem.h"
#include "treeitemcache.h"

void updateHLT(QTextCursor* cursor, int rowI, QTextCharFormat* fmt);

void clickedDocksView(PointsModel* pointsModel, ParseAndRead* parts, FileModel* fileModel)
{
    auto* parentItem = pointsModel->getRoot();

    for (auto fileName : parts->filesName()) {
        fileModel->appendRow(new QStandardItem(fileName));

        for (auto points : parts->pointData(fileName)) {
            int dotCount = points.marker.count('.');
            int depth = dotCount - 1; // subtract the final dot
            QString indent(depth * 4, ' '); // 4 spaces per level

            // qDebug() << "INFO:" << points.marker << dotCount << depth;
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
                // return -1;
            }

            // qDebug() << points.marker;
        }
    }
}

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
    pointsView->setRootIsDecorated(true);
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

    //===========================================================================================
    ParseAndRead* parts = new ParseAndRead;

    TreeItemCache cache;

    ReadFiles readFiles;
    readFiles.collectFiles();
    auto d = readFiles.result();
    for (auto i : d) {
        qDebug() << "INFO:::" << i.fileName;
        // parts->parse(i.fileName, rawText);
        parts->parse(i.fileName);
        cache.put(i.fileName);
    }
    // parts->parse(fileName);
    // cache.put(fileName);

    for (auto fileName : parts->filesName()) {
        fileModel->appendRow(new QStandardItem(fileName));

        pointsModel->setBorrowedRoot(cache.getRaw(fileName));
        auto* parentItem = pointsModel->getRoot();

        for (auto points : parts->pointData(fileName)) {
            int dotCount = points.marker.count('.');
            int depth = dotCount - 1; // subtract the final dot
            QString indent(depth * 4, ' '); // 4 spaces per level

            // qDebug() << "INFO:" << points.marker << dotCount << depth;
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
                // return -1;
            }

            // qDebug() << points.marker;
        }
    }

    pointsModel->setBorrowedRoot(cache.getRaw(fileName));


    QObject::connect(fileView->selectionModel(), &QItemSelectionModel::currentChanged,
        fileView, [&](const QModelIndex& cur, const QModelIndex& pre) {
            // rowI = index.row();
            infoLabel->setText(QString("Ви клікнули fileView: [%1, %2] = %3")
                    .arg(cur.row())
                    .arg(cur.column())
                    .arg(cur.data().toString()));
            // clickedDocksView(pointsModel, parts, fileModel);

            const QString path = fileModel->data(cur).toString();
            if (path.isEmpty())
                return;

            // береш із твого кеша і просто показуєш
            if (TreeItem* root = cache.getRaw(path)) { // cache: std::unordered_map<QString, unique_ptr<TreeItem>>
                pointsModel->setBorrowedRoot(root); // модель НЕ володіє root
                //pointsView->expandToDepth(1); // опційно
            }
        });

    FileModel::connect(pointsView, &QTreeView::clicked, &w, [&](const QModelIndex& index) {
        QString key = fileView->currentIndex().data().toString();
        QString poitnText = parts->poinText(key, index.data().toString());
        text->setText(poitnText);
    });

    if (fileModel->rowCount() > 0) {
        auto first = fileModel->index(0,0);
        fileView->setCurrentIndex(first);
    }

    return a.exec();
}
