#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "defs.h"
#include "gamesystem.h"

#include <QWidget>
#include <QtEvents>
#include <QTimer>


class GameSystem;
class FrontSight;
class ChargingFrontSight;

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    typedef void (MainWindow::*FunPtr)();

private slots:
    void onActionstartTriggered();
    void on_bt_move_clicked();
    void on_bt_fire_clicked();

    void onWaitingOperation(Side side);
    void onSimulating();
    void onUnitHurt(int damage);
    void onGameOver(Side winner);

    void aimUp();
    void aimDown();
    void turnLeft();
    void turnRight();
    void charge();

private:

    Ui::MainWindow *ui;
    GameSystem *m_gamesystem;   // backend

    // game params:
    Weapon::Type m_weapon_type;
    qreal m_aim_angle;
    FrontSight *m_fs;
    ChargingFrontSight *m_cfs;
    int m_pressed_key;
    qreal m_charge;
    QTimer m_timer;

protected:
    void keepDoing(FunPtr what);
    void destroyFS();

    // QWidget interface
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // MAINWINDOW_H
