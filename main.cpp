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
#include "pointsview.h"
#include "readfiles.h"
#include "textmodel.h"
#include "treeitem.h"
#include "treeitemcache.h"

void updateHLT(QTextCursor* cursor, int rowI, QTextCharFormat* fmt);

void allActivate(bool isActivate, QPushButton* b1, QPushButton* b2, QPushButton* b3, QPushButton* b4, QPushButton* b5)
{
    b1->setEnabled(isActivate);
    b2->setEnabled(isActivate);
    b3->setEnabled(isActivate);
    b4->setEnabled(isActivate);
    b5->setEnabled(isActivate);
}

QString currentFileViewName(QTableView* fileView)
{
    QString key = fileView->currentIndex().data().toString();
    if (key == "") {
        key = fileView->indexAt({ fileView->currentIndex().row(), 0 }).data().toString();
    }

    return key;
}

void jumpToPoint(PointsModel* pointsModel, RowBandTreeView* pointsView, QString point)
{
    QModelIndexList matches = pointsModel->match(
        pointsModel->rootIndex(), // start search
        Qt::DisplayRole,
        point, // text to find
        1, // stop after 1 match
        Qt::MatchRecursive | Qt::MatchExactly);

    if (!matches.isEmpty()) {
        qDebug() << "\n~~FOUND point: " << point;
        QModelIndex idx = matches.first();
        pointsView->expand(idx.parent()); // open the parent
        pointsView->scrollTo(idx);
        pointsView->setCurrentIndex(idx);
        pointsView->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        pointsView->clicked(idx);
    }

    else
        qDebug() << "\n~~CANT FOUND point: " << point;
}

void updateTreeView(QTableView* fileView, ParseAndRead* parts, TreeItemCache* cache, ReadFiles* readFiles, PointsModel* pointsModel, FileModel* fileModel, const QString& lastStem, const QString& firstStem, const QString& patrStem)
{
    readFiles->collectFiles(lastStem, firstStem, patrStem);
    auto d = readFiles->result();
    for (auto i : d) {
        qDebug() << "INFO:::" << i.fileName;
        // parts->parse(i.fileName, rawText);
        parts->parse(lastStem, firstStem, patrStem, i.fileName);
        cache->put(i.fileName);
        cache->putFound(i.fileName);
    }
    // parts->parse(fileName);
    // cache.put(fileName);

    for (auto fileName : parts->filesName()) {
        pointsModel->setBorrowedRoot(cache->getRaw(fileName));
        auto* parentItem = pointsModel->getRoot();
        auto* parentFoundRoot = cache->getRawFound(fileName);

        fileModel->appendRow({ new QStandardItem(fileName), new QStandardItem() });

        for (auto points : parts->pointData(fileName)) {
            int dotCount = points.marker.count('.');
            int depth = dotCount - 1; // subtract the final dot
            QString indent(depth * 4, ' '); // 4 spaces per level

            // qDebug() << "INFO:" << points.marker << dotCount << depth;
            QString val = points.marker;
            QString name = points.name;

            if (!name.isEmpty())
                parentFoundRoot->appendChild(val, name);

            auto it = pointsModel->find(parentItem, val);
            if (it == nullptr) {

                QString s = val;
                bool find = false;
                while (s.count('.') > 1) {
                    s = shortenPath(s);
                    auto itS = pointsModel->find(parentItem, s);
                    if (itS != nullptr) {
                        qDebug() << "Parent" << itS->data().toString() << " for " << val;
                        itS->appendChild(val, name);
                        find = true;
                        break;
                    }
                }

                if (!find)
                    parentItem->appendChild(val, name);

            } else {
                qDebug() << "\n\nError val it's already exist:" << val;
                // return -1;
            }

            // qDebug() << points.marker;
        }
    }

    if (fileModel->rowCount() > 0) {
        auto first = fileModel->index(0, 0);
        fileView->setCurrentIndex(first);
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
    fileModel->setHorizontalHeaderItem(1, new QStandardItem("Points with name"));

    PointsModel* pointsModel = new PointsModel(mainW);
    // pointsModel->setHorizontalHeaderItem(0, new QStandardItem("Points"));

    QTableView* fileView = new QTableView();
    fileView->setModel(fileModel);
    fileView->setAlternatingRowColors(true);

    RowBandTreeView* pointsView = new RowBandTreeView();
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

    auto* lastName = new QLineEdit("Пу");
    auto* firstName = new QLineEdit();
    auto* patronymicName = new QLineEdit("Ігор");

    nameEditLayout->addWidget(lastName);
    nameEditLayout->addWidget(firstName);
    nameEditLayout->addWidget(patronymicName);

    lastName->setPlaceholderText("Last Name");
    firstName->setPlaceholderText("First Name");
    patronymicName->setPlaceholderText("Patronymic Name");

    auto* updateButt = new QPushButton("UPDATE");
    vLayout->addWidget(updateButt);

    QPushButton* toggle = new QPushButton("SHOW ONLY FOUNDED");
    toggle->setCheckable(true);
    vLayout->addWidget(toggle);

    auto* pointLabel = new QLineEdit("1.");
    pointLabel->setPlaceholderText("Skroll to the point");
    vLayout->addWidget(pointLabel);

    QPushButton* jumpPointButt = new QPushButton("Jump to the point");
    vLayout->addWidget(jumpPointButt);

    QHBoxLayout* movePointsLayout = new QHBoxLayout();
    vLayout->addLayout(movePointsLayout);

    auto* prevButt = new QPushButton("PREV");
    auto* currButt = new QPushButton("CURRENT");
    auto* nextButt = new QPushButton("NEXT");

    movePointsLayout->addWidget(prevButt);
    movePointsLayout->addWidget(currButt);
    movePointsLayout->addWidget(nextButt);

    allActivate(false, toggle, jumpPointButt, prevButt, currButt, nextButt);

    w.show();

    //===========================================================================================
    // QString fileName = "00____myShityTestFile____00.txt";
    //===========================================================================================

    auto* parts = new ParseAndRead;
    auto* cache = new TreeItemCache;
    auto* readFiles = new ReadFiles;

    // pointsModel->setBorrowedRoot(cache.getRaw(fileName));
    QObject::connect(updateButt, &QPushButton::clicked, &w, [&](bool checked) {
        allActivate(false, toggle, jumpPointButt, prevButt, currButt, nextButt);

        if (firstName->text().isEmpty() && lastName->text().isEmpty() && patronymicName->text().isEmpty())
            qDebug() << "\nSORRY all empty";
        qDebug() << firstName->text() << lastName->text() << patronymicName->text();
        updateTreeView(fileView, parts, cache, readFiles, pointsModel, fileModel, lastName->text(), firstName->text(), patronymicName->text());

        allActivate(true, toggle, jumpPointButt, prevButt, currButt, nextButt);
    });

    QObject::connect(fileView->selectionModel(), &QItemSelectionModel::currentChanged,
        fileView, [&](const QModelIndex& cur, const QModelIndex& pre) {
            const QString path = fileModel->data(cur).toString();
            if (path.isEmpty())
                return;

            if (TreeItem* root = cache->getRawSwither(path, (toggle->text() == "SHOW ONLY FOUNDED"))) { // cache: std::unordered_map<QString, unique_ptr<TreeItem>>
                pointsModel->setBorrowedRoot(root); // модель НЕ володіє root
                // pointsView->expandToDepth(1); // опційно
            }
        });

    QObject::connect(pointsView, &RowBandTreeView::clicked, &w, [&](const QModelIndex& index) {
        auto in = index.siblingAtColumn(0).data().toString();
        qDebug() << "pointsView: " << in;

        QString key = currentFileViewName(fileView);
        QString poitnText = parts->poinText(key, in);
        text->setText(poitnText);
    });

    QObject::connect(toggle, &QPushButton::toggled, &w, [&](bool enabled) {
        if (enabled) {
            toggle->setText("SHOW ALL LIST");
            pointsView->update();
        } else {
            toggle->setText("SHOW ONLY FOUNDED");
            pointsView->update();
        }
    });

    QObject::connect(currButt, &QPushButton::clicked, &w, [&](bool checked) {
        if (pointsModel->getRoot() == nullptr)
            return;

        auto* item = cache->getRawFound(currentFileViewName(fileView));
        for (auto c : item->children()) {
            qDebug() << "child for: " << c->data().toString();
        }
        jumpToPoint(pointsModel, pointsView, item->children().at(0)->data().toString());
    });

    QObject::connect(jumpPointButt, &QPushButton::clicked, &w, [&](bool checked) {
        if (pointsModel->getRoot() == nullptr)
            return;

        jumpToPoint(pointsModel, pointsView, pointLabel->text());
    });

    return a.exec();
}
