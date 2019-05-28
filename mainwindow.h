#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QtEvents>

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

private:
    enum GameState
    {
        e_COMMON = 0,
        e_STARTED,
        e_PAUSED,
        e_SUMMARY
    };

    Ui::MainWindow *ui;
    GameSystem *m_gamesystem;
    GameState m_gamestate;
};

#endif // MAINWINDOW_H
