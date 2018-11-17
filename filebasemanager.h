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

   //additional
   void mergeDirectories(QString resultName, QStringList toMerge);
   int knownFieCopiesCounter(QCryptographicHash hash);

protected:
    FilebaseManager();
    FilebaseManager& operator =(FilebaseManager& rhs);

private:
    QString mRoot;
};

#endif // FILEBASEMANAGER_H
