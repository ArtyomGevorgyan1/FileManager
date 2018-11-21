#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "filebasemanager.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class FilebaseManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void addEntryToFilebase();
    void showEntry();
    void deleteEntry();
    void showDrive();
    void customViewer();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
