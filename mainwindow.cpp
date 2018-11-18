#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "treeitem.h"
#include "treemodel.h"
#include <QFileDialog>
#include <QDir>
#include <QIODevice>
#include <QDirIterator>
#include <QString>
#include <QList>
#include <QStringList>
#include <QVector>
#include <QVariant>
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

    addEntryToFilebase();

}

MainWindow::~MainWindow()
{
    delete ui;
}

// возвращает неправильное дерево!
void MainWindow::addEntryToFilebase()
{
    //set the path from which to pull data
    QString path = QFileDialog::getExistingDirectory(this,
                                                     "Choose the drive",
                                                     QDir::rootPath(),
                                                     QFileDialog::ReadOnly);
    QDirIterator iter(path, QDirIterator::Subdirectories);

    // set the header treeitem to hold the informaton needed to be privided to view
    QList <QVariant>* headerData = new QList <QVariant>;
    *headerData << "Name" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* header = new TreeItem(*headerData);

    // information needed to traverse the filetree
    QString prevFilePath = path;
    QVector <QPair <TreeItem*, QString>> stack;
    stack.push_back({header, path});
    // stack is never empty!

    int cnt = 0;
    while (iter.hasNext()) {

        //qDebug() << cnt << "\n";

        //useless check - stack is never empty
        //if (stack.isEmpty()) {
          //  break;
        //}

        // read the current file info and ignore curPath/. and curPath/..
        QString thisFilePath = iter.next();
        /*
        if (thisFilePath == path + "/." ||
            thisFilePath == path + "/..") {
            continue;
        }
        */
        if (thisFilePath[thisFilePath.size() - 1] == ".") {
            continue;
        }
        //qDebug() << thisFilePath <<"\n";
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

        //qDebug() << *data;

        // this block manages the parental relationships
        TreeItem* currentItem;
        TreeItem* prevParent = stack.last().first;
        //qDebug() << prevParent->data(0) <<" look\n";
        QString prevFilePath = stack.last().second;

        // проверка не работает?
        /*
        qDebug() << prevFilePath << thisFilePath <<  "\n";
        qDebug() << prevFilePath.contains(thisFilePath);
        */

        if (thisFilePath.contains(prevFilePath)) {
            currentItem = new TreeItem(*data, prevParent);
            prevParent -> appendChild(currentItem);
            stack.push_back({currentItem, thisFilePath});
        } else {
            stack.pop_back();
            currentItem = new TreeItem(*data, stack.last().first);
            stack.last().first -> appendChild(currentItem);
            stack.push_back({currentItem, thisFilePath});
        }
        cnt++;
    }
    // получили дерево, которое представляет диск

    // запишем его в файл
    qDebug() << header ->data(0);
    qDebug() << "has " << header -> childCount() << " children\n";

    //FilebaseManager::instance().writeTree(header);

    qDebug() << "SURVIVED\n";

    headerData = new QList <QVariant>;
    *headerData << "Name" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* testTree = new TreeItem(*headerData, nullptr);

    headerData = new QList <QVariant>;
    *headerData << "A" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* next = new TreeItem(*headerData, testTree);
    testTree -> appendChild(next);

    headerData = new QList <QVariant>;
    *headerData << "AA" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* next1 = new TreeItem(*headerData, next);
    next -> appendChild(next1);

    headerData = new QList <QVariant>;
    *headerData << "B" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* next2 = new TreeItem(*headerData, testTree);
    testTree -> appendChild(next2);



    FilebaseManager::instance().writeTree(header);
}

#include <QStringListModel>
#include <QListView>

// не проверял

/* в плане сделать свое меню для выбора, какой файл открыть */
void MainWindow::showEntry()
{
   QString fileName = QFileDialog::getOpenFileName(this, "select",
                                                   FilebaseManager::instance().root(),
                                                   "(*.inf)");
   TreeModel* model = new TreeModel(FilebaseManager::instance().readTree(fileName), this);
   ui ->treeList->setModel(model);
   ui -> treeList->show();
}

void MainWindow::deleteEntry()
{
    QString fileName = QFileDialog::getOpenFileName(this, "select",
                                                    FilebaseManager::instance().root(),
                                                    "(*.inf)");

}

#include <QFileSystemModel>
void MainWindow::showDrive()
{
    QString drive = QFileDialog::getExistingDirectory(this,
                                                      tr("Select Directory"),
                                                      QDir::rootPath());
    QFileSystemModel* model = new QFileSystemModel(this);
    QTreeView* view = new QTreeView;
    view -> setModel(model);
    view -> show();
}
