#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "treeitem.h"
#include "treemodel.h"
#include "filesystemdialog.h"
#include <QFileDialog>
#include <QDir>
#include <QIODevice>
#include <QDirIterator>
#include <QString>
#include <QStringListModel>
#include <QHash>
#include <QListView>
#include <QLineEdit>
#include <QInputDialog>
#include <QList>
#include <QStringList>
#include <QVector>
#include <QVariant>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QDateTime>
#include <QDate>
#include <QCryptographicHash>
#include <QDebug>
#include <QTime>
#include <QTreeView>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    FilebaseManager::instance().setRoot(QDir::currentPath());

    //addEntryToFilebase();
    //showEntry();
    //showDrive();
    //customViewer();
    addEntryToFilebase();
    //TreeItem* p = FilebaseManager::instance().readTree("Name.inf");
    //deleteEntry();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addEntryToFilebase()
{
    //set the path from which to pull data
    QString path = QFileDialog::getExistingDirectory(this,
                                                     "Choose the drive",
                                                     QDir::rootPath(),
                                                     QFileDialog::ReadOnly);
    QDirIterator iter(path, QDirIterator::Subdirectories);


    qDebug() << path << "\n";

    // set the header treeitem to hold the informaton needed to be privided to view
    QList <QVariant>* headerData = new QList <QVariant>;
    *headerData << "Name" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* header = new TreeItem(*headerData);


    QHash<QString, TreeItem*> map;
    map.insert(path, header);
    while (iter.hasNext()) {


        // собрать инф
        QString thisFilePath = iter.next();

        if (thisFilePath[thisFilePath.size() - 1] == ".") {
            continue;
        }
        qDebug() << thisFilePath;

        QFileInfo info(thisFilePath);
        QList <QVariant>* data = new QList <QVariant>;

        // this block sets the data list
        QString hashValue;
        if (info.isFile()) {
            QCryptographicHash calculateMd5(QCryptographicHash::Md5);
            QFile file;
            file.setFileName(iter.filePath());
            if (!file.open(QIODevice::ReadOnly)) {
                hashValue = "failed";
            }
            calculateMd5.addData(file.readAll());
            hashValue = calculateMd5.result().toHex();
            file.close();
        }

        *data << info.fileName() << info.birthTime().time().toString("hh:mm:ss")
              << info.lastModified().time().toString("hh:mm:ss")
              << info.suffix()
              << info.isDir();
        if (info.isDir()) {
            *data << "null";
        } else if (info.isFile()) {
            *data << hashValue;
        }


        QString parentPath = thisFilePath;
        //убрать до последнего слеша
        for (int i = thisFilePath.size() - 1; i >= 0; i--) {
            if (thisFilePath[i] == "/" && i == 0) {
                parentPath.replace(i, 1, " ");
            } else if (thisFilePath[i] != "/") {
                parentPath.replace(i, 1, " ");
            } else {
                break;
            }
        }
        parentPath = parentPath.trimmed();
        if (parentPath[parentPath.size() - 1] == "/") {
            // папка
            parentPath.replace(parentPath.size() - 1, 1, " ");
        }
        parentPath = parentPath.trimmed();


        TreeItem* parent = map[parentPath];
        TreeItem* currentItem = new TreeItem(*data, parent);
        parent ->appendChild(currentItem);
        map.insert(thisFilePath, currentItem);

    }
    // получили дерево, которое представляет диск

    // запишем его в файл
     FilebaseManager::instance().writeTree(header);
}

void MainWindow::showEntry()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("User name:"), QLineEdit::Normal,
                                         FilebaseManager::instance().root(), &ok);

    QDir dir(FilebaseManager::instance().root());
    if (!ok || text.isEmpty()) {
        return;
    }
    if (dir.exists(text + ".inf")) {
        TreeItem* tree = FilebaseManager::instance().readTree(text + ".inf");

        /*
        qDebug() << tree ->data(0);
        for (int i = 0; i < tree->childCount(); i++) {
            qDebug() << tree ->child(i)->data(0);
        }
        */

        TreeModel* model = new TreeModel(tree);
        ui ->treeList->setModel(model);
        ui -> treeList->show();
    }
}

void MainWindow::deleteEntry()
{
    QString fileName = QFileDialog::getOpenFileName(this, "select",
                                                    FilebaseManager::instance().root(),
                                                    "(*.inf)");
    FilebaseManager::instance().removeFile(fileName);

}

void MainWindow::showDrive()
{
    QString drive = QFileDialog::getExistingDirectory(this,
                                                      tr("Select Directory"),
                                                      QDir::rootPath());
    QFileSystemModel* model = new QFileSystemModel(this);
    model -> setRootPath(drive);
    QTreeView* view = new QTreeView;
    view -> setModel(model);
    view -> show();
}

void MainWindow::customViewer()
{
    FileSystemDialog* dialog = new FileSystemDialog(this);
    dialog->show();
}
