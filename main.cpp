#include "mainwindow.h"

#include <QApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
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

void clickedDocksView(ParseAndRead* parts, TreeItemCache* cache, ReadFiles* readFiles, PointsModel* pointsModel, FileModel* fileModel, const QString& lastStem, const QString& firstStem, const QString& patrStem)
{
    readFiles->collectFiles(lastStem, firstStem, patrStem);
    auto d = readFiles->result();
    for (auto i : d) {
        qDebug() << "INFO:::" << i.fileName;
        // parts->parse(i.fileName, rawText);
        parts->parse(i.fileName);
        cache->put(i.fileName);
    }
    // parts->parse(fileName);
    // cache.put(fileName);

    for (auto fileName : parts->filesName()) {
        fileModel->appendRow(new QStandardItem(fileName));

        pointsModel->setBorrowedRoot(cache->getRaw(fileName));
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

    QSplitter* splitter = new QSplitter();

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

    vLayout->addLayout(hLayout);
    hLayout->addWidget(splliter);

    QHBoxLayout* nameEditLayout = new QHBoxLayout();
    vLayout->addLayout(nameEditLayout);

    auto* firstName = new QLineEdit();
    auto* lastName = new QLineEdit();
    auto* patronymicName = new QLineEdit();
    nameEditLayout->addWidget(firstName);
    nameEditLayout->addWidget(lastName);
    nameEditLayout->addWidget(patronymicName);

    firstName->setPlaceholderText("First Name");
    lastName->setPlaceholderText("Last Name");
    patronymicName->setPlaceholderText("Patronymic Name");

    auto* updateButt = new QPushButton("UPDATE");
    vLayout->addWidget(updateButt);

    w.show();

    //===========================================================================================
    // QString fileName = "00____myShityTestFile____00.txt";
    //===========================================================================================

    auto* parts = new ParseAndRead;
    auto* cache = new TreeItemCache;
    auto* readFiles = new ReadFiles;

    // pointsModel->setBorrowedRoot(cache.getRaw(fileName));
    QObject::connect(updateButt, &QPushButton::clicked, &w, [&](bool checked) {
        if (firstName->text().isEmpty() && lastName->text().isEmpty() && patronymicName->text().isEmpty())
            qDebug() << "\nSORRY all empty";
        qDebug() << firstName->text() << lastName->text() << patronymicName->text();
        clickedDocksView(parts, cache, readFiles, pointsModel, fileModel, lastName->text(), firstName->text(), patronymicName->text());
    });

    QObject::connect(fileView->selectionModel(), &QItemSelectionModel::currentChanged,
        fileView, [&](const QModelIndex& cur, const QModelIndex& pre) {
            const QString path = fileModel->data(cur).toString();
            if (path.isEmpty())
                return;

            if (TreeItem* root = cache->getRaw(path)) { // cache: std::unordered_map<QString, unique_ptr<TreeItem>>
                pointsModel->setBorrowedRoot(root); // модель НЕ володіє root
                // pointsView->expandToDepth(1); // опційно
            }
        });

    QObject::connect(pointsView, &QTreeView::clicked, &w, [&](const QModelIndex& index) {
        QString key = fileView->currentIndex().data().toString();
        QString poitnText = parts->poinText(key, index.data().toString());
        text->setText(poitnText);
    });

    // if (fileModel->rowCount() > 0) {
    //     auto first = fileModel->index(0, 0);
    //     fileView->setCurrentIndex(first);
    // }

    return a.exec();
}
