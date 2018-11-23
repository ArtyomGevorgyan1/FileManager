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



   void writeTree(TreeItem* parent, QString driveName) const;

   TreeItem* readTree(QString driveName) const;


   void removeFile(QString fileName) const;

   QStringList readLines(QString driveName) const;

   QString readAll(QString driveName) const;


   void mergeDirectories(QString resultName, QStringList toMerge);
   int knownFieCopiesCounter(QByteArray hash);
   //QList <QString> searchFor(QString searchTarget, QList <QString> params) const;

protected:
    FilebaseManager();
    FilebaseManager& operator =(FilebaseManager& rhs);

private:
    QString mRoot;
};

#endif // FILEBASEMANAGER_H
