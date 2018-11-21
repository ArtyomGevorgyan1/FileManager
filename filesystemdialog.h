#ifndef QFILESYSTEMMODELDIALOG_H
#define QFILESYSTEMMODELDIALOG_H

#include <QDialog>
#include <QFileSystemModel>

namespace Ui {
class FileSystemDialog;
}

class FileSystemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FileSystemDialog(QWidget *parent = 0);
    ~FileSystemDialog();

private slots:

    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::FileSystemDialog *ui;
    QFileSystemModel *dirModel;
    QFileSystemModel *fileModel;
};

#endif // QFILESYSTEMMODELDIALOG_H
