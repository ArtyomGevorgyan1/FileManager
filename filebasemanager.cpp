#include "filebasemanager.h"
#include <QIODevice>
#include <QTextStream>
#include <QDir>
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

void FilebaseManager::writeTree(TreeItem* parent, QString driveName) const
{
    //QString driveName = parent -> data(0).toString();
    QFile file(mRoot + "/" + driveName + ".inf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "failed to open file\n";
    }
    QTextStream in(&file);

    QList <QVariant> *data  = new QList <QVariant>;
    *data << "name " << "cost ";
    TreeItem* p = new TreeItem(*data);
    TreeItem* pp = new TreeItem(*data, p);
    p -> appendChild(pp);
    TreeItem* ppp = new TreeItem(*data, pp);
    pp -> appendChild(ppp);

    QVector <TreeItem*> stack;
    QHash <TreeItem*, int> m;
    QSet <TreeItem*> vis;
    stack.push_back(parent);
    while (!stack.isEmpty()) {
        TreeItem* cur = stack.last();
        if (vis.find(cur) == vis.end()) {

            for (int i = 0; i < stack.size(); i++) {
                in << " ";
            }
            in << cur << "\n";
            vis.insert(cur);
        }

        if (cur->childCount() == 0 || (m.find(cur) != m.end() && m[cur] >= cur ->childCount())) {
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
    file.close();
}

TreeItem* FilebaseManager::readTree(QString driveName) const
{
    QFile file(mRoot + "/" + driveName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open file\n";
    }

    QTextStream out(&file);

    QStringList lines;
    while (!out.atEnd()) {
        QString line = out.readLine();
        lines.append(line);
    }

    QList <QVariant> list;
    list << "Name" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* parent = new TreeItem(list, nullptr);

    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 1;

    int number = 0;

    while (number < lines.count()) {

        int position = 0;
        while (position < lines[number].length()) {
            if (lines[number].at(position) != ' ')
                break;
            position++;
        }

        qDebug() << "Current idenntation: " << position <<"\n";

        QString lineData = lines[number].mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings1 = lineData.split(" ", QString::SkipEmptyParts);
            QStringList columnStrings;
            foreach(QString item, columnStrings1) {
                QString tmp;
                for (int j = 0; j < item.size(); j++) {
                    if (item.at(j) == "*") {
                        tmp += ".";
                        continue;
                    }
                    if (item.at(j) == "_") {
                        tmp += " ";
                        continue;
                    }
                    else {
                        tmp += item.at(j);
                    }

                }
                columnStrings << tmp;
                qDebug() << tmp;
            }

            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];
            if (position > indentations.last()) {
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

            parents.last()->appendChild(new TreeItem(columnData, parents.last()));


        }

        ++number;
    }
    file.close();
    return parent -> child(0);

}

QStringList FilebaseManager::readLines(QString driveName) const
{
    QFile file(mRoot + "/" + driveName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "failed to open file\n";
        return QStringList();
    }

    QTextStream out(&file);

    QStringList lines;
    while (!out.atEnd()) {
        QString line = out.readLine();
        lines.append(line);
    }
    return lines;
}

void FilebaseManager::removeFile(QString fileName) const {
    QDir dir(mRoot);
    if (dir.exists(fileName)) {
        dir.remove(fileName);
    } else {
        qDebug() << "no such file\n";
        return;
    }
}

QString FilebaseManager::readAll(QString driveName) const
{
    QString path = mRoot + "/" + driveName + ".inf";
    qDebug() << path;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "cannot open the file\n";
        return QString();
    }
    QString result = file.readAll();
    file.close();
    return result;
}

int FilebaseManager::knownFieCopiesCounter(QByteArray hash)
{
    QDir dir(mRoot);
    QFileInfoList buffer = dir.entryInfoList();
    buffer.pop_front();
    buffer.pop_front();

    QFileInfoList list;

    foreach(QFileInfo info, buffer) {
        if (info.isDir()) {
            continue;
        }
        QString f = info.suffix();
        if (f == "inf") {
            list << info;

        }
    }

    foreach(QFileInfo info, list) {
        qDebug() << info.fileName();
    }

    int counter = 0;
    QString searchFor = hash.toHex();
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
    qDebug() << counter;
    return counter;
}

void FilebaseManager::mergeDirectories(QString resultName, QStringList toMerge)
{


    QVector <TreeItem*> roots;

    foreach (QString item, toMerge) {
        TreeItem* root = readTree(item);
        roots.push_back(root);
    }

    QList <QVariant> data;
    data << "Name" << "CreatedTime" << "ModifiedTime" << "Extension" << "IsDir" << "HashValue";
    TreeItem* parent = new TreeItem(data);

    foreach(TreeItem* item, roots) {
        for (int i = 0; i < item -> childCount(); i++ ) {
            parent -> appendChild(item -> child(i));
            item -> child(i) -> setParent(parent);
        }
        delete item;
    }

    writeTree(parent, resultName);
}

