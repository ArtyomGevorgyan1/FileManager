#include "filebasemanager.h"
#include <QIODevice>
#include <QTextStream>
#include <QDebug>
#include <QStringList>
#include <QHash>

FilebaseManager::FilebaseManager()
{

}

FilebaseManager::~FilebaseManager()
{

}

FilebaseManager& FilebaseManager::instance()
{
    static FilebaseManager singleton;
    return singleton;
}

void FilebaseManager::setRoot(QString root)
{
    mRoot = root;
}

QString FilebaseManager::root() const
{
    return mRoot;
}


QTextStream& operator << (QTextStream& stream, TreeItem* item)
{
    for (int i = 0; i < item->columnCount(); i++) {
        stream << item -> data(i).toString();
        stream << " ";
    }
    return stream;
}

void test(TreeItem* item, int d) {

    for (int i = 0;i < item -> childCount(); i++) {
        test(item ->child(i), d+1);
    }
    qDebug() << item -> data(0) << d << "\n";
}

// работает правильно! - ytn
/*
void FilebaseManager::writeTree(TreeItem* parent) const
{
    QString driveName = parent -> data(0).toString();
    QFile file(mRoot + "/" + driveName + ".inf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "failed to open file\n";
    }

    qDebug() << "ENDED TEST\n";

    QTextStream in(&file);


    QVector <QPair<int, TreeItem*>> stack;
    QHash<TreeItem*, int> lastChildWritten;
    stack.push_back({0, parent});
    QHash<TreeItem*, bool> isWritten;
    isWritten.insert (parent, true);


    while (1) {

        if (stack.isEmpty()) {
            break;
        }

        int curIndent = stack.last().first;
        TreeItem* curItem = stack.last().second;
        stack.pop_back();

        if (isWritten.find(curItem) !=isWritten.end()) {
            for (int i = 0; i <curIndent; i++) {
                in << " ";
            }
            in << curItem << "\n";

            isWritten.insert(curItem, true);
        }


        int currentChildIndex = 0;
        if (lastChildWritten.find(curItem) != lastChildWritten.end()) {
            currentChildIndex = lastChildWritten[curItem];
        } else {
            lastChildWritten.insert(curItem, 0);
        }

        if (curItem -> childCount() > 0 && currentChildIndex < curItem -> childCount()) {
            qDebug() << "pushing" << curIndent + 1 << "\n";
            stack.push_back({curIndent + 1, curItem -> child(currentChildIndex)});
            lastChildWritten[curItem]++;
        } else if (parent != curItem -> parentItem()){
            stack.push_back({curIndent - 1, curItem -> parentItem()});
        }
    }

    file.close();
}
    */


void FilebaseManager::writeTree(TreeItem* parent) const
{
    QString driveName = parent -> data(0).toString();
    QFile file(mRoot + "/" + driveName + ".inf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "failed to open file\n";
        //return;
    }

    qDebug() << "OPENED THE FILE\n";
    QTextStream in(&file);

    //дерево правильное 100%
    //test(parent, 0);

    QList <QVariant> *data  = new QList <QVariant>;
    *data << "name " << "cost ";
    TreeItem* p = new TreeItem(*data);
    TreeItem* pp = new TreeItem(*data, pp);
    p -> appendChild(pp);
    TreeItem* ppp = new TreeItem(*data, pp);
    pp -> appendChild(ppp);

    QVector <TreeItem*> stack;
    QHash <TreeItem*, int> m;
    QSet <TreeItem*> vis;
    // тест
    stack.push_back(p);
    int i = 0;
    while (!stack.isEmpty()) {
        qDebug() << i++;
        TreeItem* cur = stack.last();
        if (vis.find(cur) == vis.end()) {

            for (int i = 0; i < stack.size(); i++) {
                in << " ";
            }
            in << cur << "\n";
            qDebug() << cur -> data(0);

            vis.insert(cur);
        }

        if (cur->childCount() == 0 || (m.find(cur) != m.end() && m[cur] >= cur ->columnCount())) {
            stack.pop_back();
        } else {
            if (m.find(cur) == m.end()) {
                m.insert(cur, 0);
            }

            if (m[cur] < cur -> childCount()) {
                stack.push_back(cur -> child(m[cur]));
                m[cur]++;
            }
        }
    }
    qDebug()<<"Lllll\n";

    file.close();
}


// работает - нет
/*
TreeItem* FilebaseManager::readTree(QString driveName) const
{
    QFile file(mRoot + "/" + driveName);
    qDebug() << "HERE " << mRoot + "/" + driveName;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open file\n";
    }

    QTextStream out(&file);

    QStringList lines;
    while (!out.atEnd()) {
        QString line = out.readLine();
        lines.append(line);
    }


    // заполним сейча header, потом пропустим певую итерацию цикла
    QList <QVariant> list;
    list << "Name" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* parent = new TreeItem(list, nullptr);

    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    // начинаем с единицы, потому что перввая (0-я) строка уже записана
    int number = 1;

    while (number < lines.count()) {

        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }

        ++number;
    }
    file.close();
    return parent;
}*/

TreeItem* FilebaseManager::readTree(QString driveName) const
{
    QFile file(mRoot + "/" + driveName);
    qDebug() << "HERE " << mRoot + "/" + driveName;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open file\n";
    }

    QTextStream out(&file);

    QStringList lines;
    while (!out.atEnd()) {
        QString line = out.readLine();
        lines.append(line);
    }


    // заполним сейча header, потом пропустим певую итерацию цикла
    QList <QVariant> list;
    list << "Name" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* parent = new TreeItem(list, nullptr);

    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    // начинаем с единицы, потому что перввая (0-я) строка уже записана
    int number = 1;

    while (number < lines.count()) {

        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }

        ++number;
    }
    file.close();
    return parent;
}

#include <QDir>
void FilebaseManager::removeFile(QString fileName) const {
    QDir dir(mRoot);
    if (dir.exists(fileName)) {
        dir.remove(fileName);
    } else {
        qDebug() << "no such file\n";
    }
}

// не проверял
int FilebaseManager::knownFieCopiesCounter(QCryptographicHash hash)
{
    QDir dir(mRoot);
    QFileInfoList list = dir.entryInfoList();
    int counter = 0;
    QString searchFor = hash.result().toHex();
    foreach(QFileInfo info, list) {
        QString curFileName = info.fileName();
        QFile file(curFileName);
        if (!file.open(QIODevice::ReadOnly)) {
            //
        }
        QTextStream stream(&file);
        QString text = stream.readAll();
        counter += text.count(searchFor);
    }
    return counter;
}


void FilebaseManager::mergeDirectories(QString resultName, QStringList toMerge)
{
    // записать в новый файл содержимое файлов из списка
}
