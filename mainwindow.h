#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QtEvents>
#include "defs.h"

class GameSystem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionstart_triggered();

    void on_bt_test_clicked();

    void listenOperation();

private:

    Ui::MainWindow *ui;
    GameSystem *m_gamesystem;
};

#endif // MAINWINDOW_H
