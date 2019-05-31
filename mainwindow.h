#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QtEvents>
#include "defs.h"
#include "gamesystem.h"

class GameSystem;
class FrontSight;

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

    void on_bt_move_clicked();

    void onWaitingOperation(Side side);

    void onSimulating();

    void on_bt_fire_clicked();

    void onUnitHurt(int damage);

    void onGameOver(Side winner);

private:

    Ui::MainWindow *ui;
    GameSystem *m_gamesystem;   // backend

    // game params:
    Weapon::Type m_weapon_type;
    qreal m_aim_angle;
    FrontSight *m_fs;

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // MAINWINDOW_H
