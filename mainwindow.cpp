#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "frontsight.h"

#include <Box2D/Box2D.h>
#include <QGraphicsView>
#include <QMessageBox>
#include <QRadioButton>
#include <QtMath>


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_gamesystem(new GameSystem),
    m_fs(nullptr), m_cfs(nullptr),
    m_bt_group(this), m_map_name(""), m_game_name("Warzone")
{
    ui->setupUi(this);
    setWindowTitle(m_game_name);
    // button logic
    m_bt_group.addButton(ui->rbt_bazooka);
    m_bt_group.addButton(ui->rbt_grenade);
    m_bt_group.addButton(ui->rbt_shotgun);
    m_bt_group.setId(ui->rbt_bazooka, 0);
    m_bt_group.setId(ui->rbt_grenade, 1);
    m_bt_group.setId(ui->rbt_shotgun, 2);
    connect(ui->rbt_bazooka, &QRadioButton::clicked, this, [this](){m_weapon_type = Weapon::e_BAZOOKA;});
    connect(ui->rbt_grenade, &QRadioButton::clicked, this, [this](){m_weapon_type = Weapon::e_GRENADE;});
    connect(ui->rbt_shotgun, &QRadioButton::clicked, this, [this](){m_weapon_type = Weapon::e_SHOTGUN;});

    connect(ui->bt_start, SIGNAL(clicked()), ui->actionstart, SLOT(trigger()));
    connect(ui->actionstart, SIGNAL(triggered(bool)), this, SLOT(onActionstartTriggered()));
    connect(ui->bt_skip, SIGNAL(clicked()), this, SLOT(onSkip()));
    connect(ui->comboBox, &QComboBox::currentTextChanged, this, [this](const QString &map_name){ m_map_name = map_name; });

    // graphics view
    auto &view = *ui->graphics_view;
    view.setScene(m_gamesystem->getScene());
    view.setRenderHint(QPainter::Antialiasing);
    view.setBackgroundBrush(Qt::white);
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setTransformationAnchor(QGraphicsView::NoAnchor);
    view.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
    view.setDragMode(QGraphicsView::ScrollHandDrag);
    view.scale(0.6, -0.6);

    // game system logic
    connect(m_gamesystem, SIGNAL(initializing(bool)), this, SLOT(onInitializing(bool)));
    connect(m_gamesystem, SIGNAL(requireOperation(Side)), this, SLOT(onWaitingOperation(Side)));
    connect(m_gamesystem, SIGNAL(playerChanged(Side)), this, SLOT(onPlayerChanged(Side)));
    connect(m_gamesystem, SIGNAL(beginSimulating()), this, SLOT(onSimulating()));
    connect(m_gamesystem, SIGNAL(unitHurt(int)), this, SLOT(onUnitHurt(int)));
    connect(m_gamesystem, SIGNAL(gameOver(Side)), this, SLOT(onGameOver(Side)));
    connect(m_gamesystem, SIGNAL(setLabelPrompt(QString)), ui->label, SLOT(setText(QString)));
    connect(m_gamesystem, SIGNAL(setLCDNumber(int)), ui->lcdNumber, SLOT(display(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    m_gamesystem->deleteLater();
}

void MainWindow::onActionstartTriggered()
{
    destroyFS();
    m_pressed_key = 0;
    m_aim_angle = 0;
    ui->rbt_bazooka->click();
    m_gamesystem->start(m_map_name);
}

void MainWindow::onWaitingOperation(Side side)
{
    auto msg = QString("listening for %1 operation...").arg(side == e_RED ? "red" : "black");
    qDebug() << msg;
    ui->bt_start->setEnabled(true);
    ui->bt_skip->setEnabled(true);
    setEnableBtGroup(true);
    m_pressed_key = 0;
    Q_ASSERT(!m_fs);
    m_fs = new FrontSight;
    m_fs->setPos(m_gamesystem->getCurUnit()->pos());
    m_fs->setRotation(m_aim_angle);
    m_gamesystem->addToScene(m_fs);
}

void MainWindow::onSimulating()
{
    ui->bt_start->setEnabled(false);
    ui->bt_skip->setEnabled(false);
    setEnableBtGroup(false);    // lock
}

void MainWindow::onUnitHurt(int damage)
{
    ui->label->setText("Damage");
    ui->lcdNumber->display(damage);
}

void MainWindow::onGameOver(Side winner)
{
    QString msg = "Game Over. ";
    if (winner == e_TIE) msg += "Tie!";
    else msg += QString("%1 wins!").arg(winner == e_RED ? "Red" : "Black");
    QMessageBox::information(this, "Game Over", msg, QMessageBox::Ok);
    ui->bt_start->setEnabled(true);
}

void MainWindow::onPlayerChanged(Side)
{
    ui->rbt_bazooka->click();
}

void MainWindow::onInitializing(bool flag)
{
    if (flag)
        setWindowTitle(m_game_name + " (initializing...)");
    else
        setWindowTitle(m_game_name);
}

void MainWindow::onSkip()
{
    destroyFS();
    m_gamesystem->skipCurUnit();
}

void MainWindow::setEnableBtGroup(bool flag)
{
    foreach (auto bt, m_bt_group.buttons()) {
        bt->setEnabled(flag);
    }
}

void MainWindow::switchWeapon()
{
    int cur_id = m_bt_group.checkedId();
    Q_ASSERT(cur_id >= 0);
    cur_id = (cur_id + 1) % GameConsts::max_weapon_id;
    m_bt_group.button(cur_id)->click();
}

void MainWindow::aimUp()
{
    if (-90 < m_aim_angle && m_aim_angle < 90) m_aim_angle = qMin(m_aim_angle + 0.9, 89.99);
    else m_aim_angle = qMax(m_aim_angle - 0.9, 90.01);
    m_fs->setRotation(m_aim_angle);
}

void MainWindow::aimDown()
{
    if (-90 < m_aim_angle && m_aim_angle < 90) m_aim_angle = qMax(m_aim_angle - 0.9, -89.99);
    else m_aim_angle = qMin(m_aim_angle + 0.9, 269.99);
    m_fs->setRotation(m_aim_angle);
}

void MainWindow::turnLeft()
{
    if (-90 < m_aim_angle && m_aim_angle < 90) {
        m_aim_angle = 180 - m_aim_angle;
        m_fs->setRotation(m_aim_angle);
    }
}

void MainWindow::turnRight()
{
    if (90 < m_aim_angle && m_aim_angle < 270) {
        m_aim_angle = 180 - m_aim_angle;
        m_fs->setRotation(m_aim_angle);
    }
}

void MainWindow::charge()
{
    m_charge = qMin(m_charge + 0.01, 1.0); // 1s full
}

void MainWindow::keepDoing(MainWindow::FunPtr what)
{
    Q_ASSERT(!m_timer.isActive());
    disconnect(&m_timer, &QTimer::timeout, 0, 0);
    connect(&m_timer, &QTimer::timeout, this, what);
    m_timer.start(10);  // 100 shots per sec
}

void MainWindow::destroyFS()
{
    m_gamesystem->removeFromScene(m_fs);
    delete m_fs;
    m_fs = nullptr;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!m_gamesystem->isOperational() || m_pressed_key != 0) {
        event->ignore();
        return; // listen for the first key
    }
    m_pressed_key = event->key();
    switch (event->key()) {

    // fire
    case Qt::Key_K:
    case Qt::Key_Space:
    {
        Q_ASSERT(!m_cfs);
        m_cfs = new ChargingFrontSight;
        m_cfs->setPos(m_gamesystem->getCurUnit()->pos());
        m_cfs->setRotation(m_aim_angle);
        m_gamesystem->addToScene(m_cfs);
        m_cfs->startAnimation();
        m_charge = 0;
        keepDoing(&MainWindow::charge);
        break;
    }

    // jump
    case Qt::Key_J:
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        destroyFS();
        m_charge = 0.05;
        keepDoing(&MainWindow::charge);
        break;
    }

    // aim
    case Qt::Key_W:
    case Qt::Key_Up:
        keepDoing(&MainWindow::aimUp);
        break;

    case Qt::Key_S:
    case Qt::Key_Down:
        keepDoing(&MainWindow::aimDown);
        break;

    case Qt::Key_A:
    case Qt::Key_Left:
        turnLeft();
        break;

    case Qt::Key_D:
    case Qt::Key_Right:
        turnRight();
        break;

    // switch weapon
    case Qt::Key_Q:
        switchWeapon();
        break;

    default:
        break;
    }
    event->accept();
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if (!m_gamesystem->isOperational() || m_pressed_key != event->key()) {
        event->ignore();
        return;
    }
    m_pressed_key = 0;
    switch (event->key()) {
    // fire
    case Qt::Key_K:
    case Qt::Key_Space:
    {
        m_cfs->stopAnimation();
        m_gamesystem->removeFromScene(m_cfs);
        delete m_cfs;
        m_cfs = nullptr;
        m_timer.stop();
        destroyFS();
        auto angle = qDegreesToRadians(m_aim_angle);
        m_gamesystem->fireCurUnit(m_weapon_type, m_charge * b2Vec2(cos(angle), sin(angle)));
        break;
    }

    // jump
    case Qt::Key_J:
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        m_timer.stop();
        auto angle = qDegreesToRadians(m_aim_angle);
        m_gamesystem->moveCurUnit(m_charge * b2Vec2(cos(angle), sin(angle)));
        break;
    }

    // aim
    case Qt::Key_W:
    case Qt::Key_Up:
    case Qt::Key_S:
    case Qt::Key_Down:
        m_timer.stop();
        break;

    default:
        break;
    }
    event->accept();
}

void MainWindow::on_bt_help_clicked()
{
    QString help_text =
            "Welcome to Warzone! A turn-based battle game paying tribute to Worms.\n"
            "\n"
            "Aim: WASD\n"
            "Move: press and hold [enter] or J\n"
            "Charge: press and hold [space] or K\n"
            "Fire: release [space] or K\n"
            "Weapon select: mouse click or Q\n"
            "Skip turn: mouse click\n"
            "\n"
            "Author: James Zheng, Computer Science Department, Tsinghua University.";
    QMessageBox::information(this, "Help", help_text);
}
