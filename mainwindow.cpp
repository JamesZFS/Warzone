#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <Box2D/Box2D.h>
#include <QGraphicsView>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_gamesystem(new GameSystem)
{
    ui->setupUi(this);
    connect(ui->bt_start, SIGNAL(clicked()), ui->actionstart, SLOT(trigger()));

    // graphics view
    auto &view = *ui->graphics_view;
    view.setScene(m_gamesystem->getScene());
    view.setRenderHint(QPainter::Antialiasing);
    view.setBackgroundBrush(Qt::white);
    view.setCacheMode(QGraphicsView::CacheBackground);
//    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
//    view.setDragMode(QGraphicsView::ScrollHandDrag);
    view.scale(0.8, -0.8);

    // game system logic
    connect(m_gamesystem, SIGNAL(requireOperation(Side)), this, SLOT(onWaitingOperation(Side)));
    connect(m_gamesystem, SIGNAL(beginSimulating()), this, SLOT(onSimulating()));
    connect(m_gamesystem, SIGNAL(unitHurt(int)), this, SLOT(onUnitHurt(int)));
    connect(m_gamesystem, SIGNAL(gameOver(Side)), this, SLOT(onGameOver(Side)));
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
    ui->label->setText("moving...");
    m_gamesystem->moveCurUnit(b2Vec2(randf(-1, 1), randf(0, 2)));
//    m_gamesystem->moveCurUnit(b2Vec2(1, 1));
}

void MainWindow::onWaitingOperation(Side side)
{
    auto msg = QString("listening for %1 operation...").arg(side == e_RED ? "red" : "black");
    qDebug() << msg;
    ui->label->setText("listening...");
    ui->bt_move->setEnabled(true);
    ui->bt_fire->setEnabled(true);
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
    ui->label->setText("A unit hurt");
    ui->lcdNumber->display(damage);
}

void MainWindow::onGameOver(Side winner)
{
    QString msg = "Game Over. ";
    if (winner == e_TIE) msg += "Tie!";
    else msg += QString("%1 wins!").arg(winner == e_RED ? "Red" : "Black");
    QMessageBox::information(this, "Game Over", msg, QMessageBox::Ok);
}
