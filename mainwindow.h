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
    //ok
    void addEntryToFilebase();
    // ok
    void showEntry();
    // ok
    void deleteEntry();
    // ok
    void showDrive();

    void customViewer();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
