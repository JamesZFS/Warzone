#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamesystem.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow),
    m_gamesystem(new GameSystem),
    m_gamestate(e_COMMON)
{
    ui->setupUi(this);
    connect(ui->bt_start, SIGNAL(clicked()), ui->actionstart, SLOT(trigger()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionstart_triggered()
{
    m_gamestate = e_STARTED;
}
