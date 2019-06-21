#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "defs.h"
#include "gamesystem.h"

#include <QWidget>
#include <QtEvents>
#include <QTimer>
#include <QButtonGroup>


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
    void onWaitingOperation(Side side);
    void onSimulating();
    void onUnitHurt(int damage);
    void onGameOver(Side winner);
    void onPlayerChanged(Side cur_side);
    void onInitializing(bool flag);
    void onSkip();

    void aimUp();
    void aimDown();
    void turnLeft();
    void turnRight();
    void charge();

    void on_bt_help_clicked();

private:
    Ui::MainWindow *ui;
    GameSystem *m_gamesystem;   // backend

    // game params:
    Weapon::Type m_weapon_type;
    qreal m_aim_angle;

    // views:
    FrontSight *m_fs;
    ChargingFrontSight *m_cfs;

    // inputs:
    int m_pressed_key;
    qreal m_charge;
    QTimer m_timer;
    QButtonGroup m_bt_group;
    QString m_map_name;
    const QString m_game_name;

protected:
    void keepDoing(FunPtr what);
    void destroyFS();
    void setEnableBtGroup(bool flag);
    void switchWeapon();

    // QWidget interface
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
};

#endif // MAINWINDOW_H
