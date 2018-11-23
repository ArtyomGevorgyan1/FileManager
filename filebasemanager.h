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


    // ok
   void writeTree(TreeItem* parent, QString driveName) const;
   //ok
   TreeItem* readTree(QString driveName) const;

   // ok
   void removeFile(QString fileName) const;
   // ok
   QStringList readLines(QString driveName) const;
   // ok
   QString readAll(QString driveName) const;

   //additional - потом
   void mergeDirectories(QString resultName, QStringList toMerge);
   int knownFieCopiesCounter(QByteArray hash);
   //QList <QVariant> searchFor() const;

protected:
    FilebaseManager();
    FilebaseManager& operator =(FilebaseManager& rhs);

private:
    QString mRoot;
};

#endif // FILEBASEMANAGER_H
