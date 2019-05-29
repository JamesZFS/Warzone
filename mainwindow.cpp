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
    view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
//    view.setDragMode(QGraphicsView::ScrollHandDrag);
    view.scale(5, -5);

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

void MainWindow::on_bt_test_clicked()
{
    if (!m_gamesystem->isOperational()) return;
    qDebug("try to move...");
    m_gamesystem->moveCurUnit(b2Vec2(randf(-10, 10), randf(-10, 10)));
}

void MainWindow::listenOperation()
{
    qDebug("simulation ends. listening for operation...");
}
