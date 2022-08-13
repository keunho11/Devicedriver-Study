#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(2);
    pTab2SocketClinet = new Tab2SocketClinet(ui->pTab2);
    ui->pTab2->setLayout(pTab2SocketClinet->layout());

    pTab3ControlPannel = new Tab3ControlPannel(ui->pTab3);
    ui->pTab3->setLayout(pTab3ControlPannel->layout());
    connect(pTab2SocketClinet,SIGNAL(sigTab3RecvData(QString)),pTab3ControlPannel,SLOT(slotTab3RecvData(QString)));
    connect(pTab3ControlPannel,SIGNAL(sigSocketSendData(QString)), pTab2SocketClinet,SLOT(slotSocketSendData(QString)));
}

MainWidget::~MainWidget()
{
    delete ui;
}

