#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "frontsight.h"

#include <Box2D/Box2D.h>
#include <QGraphicsView>
#include <QMessageBox>
#include <QtMath>


MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_gamesystem(new GameSystem),
    m_fs(nullptr)
{
    ui->setupUi(this);
    connect(ui->bt_start, SIGNAL(clicked()), ui->actionstart, SLOT(trigger()));

    // graphics view
    auto &view = *ui->graphics_view;
    view.setScene(m_gamesystem->getScene());
    view.setRenderHint(QPainter::Antialiasing);
    view.setBackgroundBrush(Qt::white);
    view.setCacheMode(QGraphicsView::CacheBackground);
    view.setTransformationAnchor(QGraphicsView::NoAnchor);
    view.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
//    view.setDragMode(QGraphicsView::ScrollHandDrag);
    view.scale(0.8, -0.8);

    // game system logic
    connect(m_gamesystem, SIGNAL(requireOperation(Side)), this, SLOT(onWaitingOperation(Side)));
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

void MainWindow::on_actionstart_triggered()
{
    m_gamesystem->start();
}

void MainWindow::on_bt_move_clicked()
{
    if (!m_gamesystem->isOperational()) return;
//    ui->label->setText("moving...");
//    m_gamesystem->moveCurUnit(b2Vec2(randf(-1, 1), randf(0, 2)));
//    m_gamesystem->moveCurUnit(b2Vec2(1, 1));
}

void MainWindow::onWaitingOperation(Side side)
{
    auto msg = QString("listening for %1 operation...").arg(side == e_RED ? "red" : "black");
    qDebug() << msg;
    ui->bt_move->setEnabled(true);
    ui->bt_fire->setEnabled(true);
    m_weapon_type = Weapon::e_BAZOOKA;
    m_aim_angle = 0;
    Q_ASSERT(!m_fs);
    m_fs = new FrontSight(m_gamesystem->getCurUnit()->pos());
    m_gamesystem->addToScene(m_fs);
}

void MainWindow::onSimulating()
{
    ui->bt_move->setEnabled(false);
    ui->bt_fire->setEnabled(false);
}

void MainWindow::on_bt_fire_clicked()
{
    if (!m_gamesystem->isOperational()) return;
    ui->label->setText("firing...");
    m_gamesystem->fireCurUnit(Weapon::e_BAZOOKA, b2Vec2(randf(-0.1, 0.1), randf(1, 2)));
    //    m_gamesystem->fireCurUnit(Weapon::e_BAZOOKA, b2Vec2(1, 1));
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
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!m_gamesystem->isOperational()) return;

    auto angle = qDegreesToRadians(m_aim_angle);
    switch (event->key()) {

    // fire
    case Qt::Key_K:
    case Qt::Key_Space:
        m_gamesystem->removeFromScene(m_fs);
        delete m_fs;
        m_fs = nullptr;
        m_gamesystem->fireCurUnit(m_weapon_type, 2 * b2Vec2(cos(angle), sin(angle)));
        break;

    // jump
    case Qt::Key_J:
    case Qt::Key_Return:
    case Qt::Key_Enter:
        m_gamesystem->removeFromScene(m_fs);
        delete m_fs;
        m_fs = nullptr;
        m_gamesystem->moveCurUnit(2 * b2Vec2(cos(angle), sin(angle)));
        break;

    // aim
    case Qt::Key_W:
        m_aim_angle += 10;
        m_fs->setAngle(m_aim_angle);
        break;
    case Qt::Key_S:
        m_aim_angle -= 10;
        m_fs->setAngle(m_aim_angle);
        break;

    case Qt::Key_A:
//        if (m_aim_angle > 90 || m_aim_angle < -90) break;
        m_aim_angle = 180 - m_aim_angle;
        m_fs->setAngle(m_aim_angle);
        break;

    case Qt::Key_D:
//        if (m_aim_angle < 90 || m_aim_angle > -90) break;
        m_aim_angle = 180 - m_aim_angle;
        m_fs->setAngle(m_aim_angle);
        break;

    default:
        break;
    }
}
