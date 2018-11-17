#include "filebasemanager.h"
#include <QIODevice>
#include <QTextStream>
#include <QDebug>
#include <QStringList>

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

void FilebaseManager::writeTree(TreeItem* parent) const
{
    QString driveName = parent -> data(0).toString();
    QFile file(mRoot + "/" + driveName + ".inf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "failed to open file\n";
    }

    QTextStream in(&file);
    QVector <QPair <int, TreeItem*>> reverseStack;
    reverseStack.push_back({0, parent});

    while (1) {
        if (reverseStack.isEmpty()) {
            break;
        }
        QPair <int, TreeItem*> currentPair = reverseStack.last();
        reverseStack.pop_back();
        for (int i = 0; i < currentPair.first; i++) {
            in << " ";
        }
        //in << *(currentPair.second) << "\n";

        if (currentPair.second ->childCount() > 0) {
            for (int i = currentPair.second->childCount(); i >= 0; i--) {
                reverseStack.push_back({currentPair.first + 1, currentPair.second->child(i)});
            }
        }
    }

}


// тут не проверял
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

    TreeItem* parent;

    QList<TreeItem*> parents;
    QList<int> indentations;
    parents << parent;
    indentations << 0;

    int number = 0;

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
