#ifndef FILEBASEMANAGER_H
#define FILEBASEMANAGER_H

#include "treeitem.h"
#include <QString>
#include <QCryptographicHash>

class FilebaseManager
{
public:
    static FilebaseManager& instance();
    ~FilebaseManager();

    void setRoot(QString root);
    QString root() const;


   void writeTree(TreeItem* parent) const;
   TreeItem* readTree(QString driveName) const;
   void removeFile(QString fileName) const;
   QStringList readLines(QString driveName) const;
   QString readAll(QString driveName) const;

   //additional - потом
   void mergeDirectories(QString resultName, QStringList toMerge);
   int knownFieCopiesCounter(QCryptographicHash hash);
   //QList <QVariant> searchFor() const;

protected:
    FilebaseManager();
    FilebaseManager& operator =(FilebaseManager& rhs);

private:
    QString mRoot;
};

#endif // FILEBASEMANAGER_H
