#include "filesystemdialog.h"
#include "ui_filesystemdialog.h"

FileSystemDialog::FileSystemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileSystemDialog)
{
    ui->setupUi(this);

    QString mPath = "C:/";


    dirModel = new QFileSystemModel(this);

    dirModel->setFilter(QDir::NoDotAndDotDot |
                        QDir::AllDirs);

    dirModel->setRootPath(mPath);

    ui->treeView->setModel(dirModel);



    fileModel = new QFileSystemModel(this);

    fileModel->setFilter(QDir::NoDotAndDotDot |
                        QDir::Files);

    fileModel->setRootPath(mPath);

    ui->listView->setModel(fileModel);
}

FileSystemDialog::~FileSystemDialog()
{
    delete ui;
}

void FileSystemDialog::on_treeView_clicked(const QModelIndex &index)
{
    QString mPath = dirModel->fileInfo(index).absoluteFilePath();
    ui->listView->setRootIndex(fileModel->setRootPath(mPath));
}
