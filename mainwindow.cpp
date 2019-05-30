#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamesystem.h"
#include <Box2D/Box2D.h>
#include <QGraphicsView>

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
    connect(m_gamesystem, SIGNAL(requireOperation(Side)), this, SLOT(listenOperation()));
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
    qDebug("moving...");
    m_gamesystem->moveCurUnit(b2Vec2(randf(-1, 1), randf(0, 2)));
//    m_gamesystem->moveCurUnit(b2Vec2(1, 1));
}

void MainWindow::listenOperation()
{
    qDebug("listening for operation...");
}

void MainWindow::on_bt_fire_clicked()
{
    if (!m_gamesystem->isOperational()) return;
    qDebug("firing...");
    m_gamesystem->fireCurUnit(Weapon::e_BAZOOKA, b2Vec2(randf(-2, 2), randf(0, 2)));
//    m_gamesystem->fireCurUnit(Weapon::e_BAZOOKA, b2Vec2(1, 1));
}
