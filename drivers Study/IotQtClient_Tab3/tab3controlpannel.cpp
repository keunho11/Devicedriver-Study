#include "tab3controlpannel.h"
#include "ui_tab3controlpannel.h"

Tab3ControlPannel::Tab3ControlPannel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab3ControlPannel)
{
    ui->setupUi(this);
    connect(ui->pPBLamp,SIGNAL(clicked(bool)),this,SLOT(slotLampOnOff(bool)));
    connect(ui->pPBPlug,SIGNAL(clicked(bool)),this,SLOT(slotPlugOnOff(bool)));
}

Tab3ControlPannel::~Tab3ControlPannel()
{
    delete ui;
}
void Tab3ControlPannel::slotLampOnOff(bool check)
{
    if(check)
    {
        ui->pPBLamp->setChecked(false);
        emit sigSocketSendData("[19]LAMPON");
    }
    else {
        ui->pPBLamp->setChecked(true);
        emit sigSocketSendData("[19]LAMPOFF");
    }
}
void Tab3ControlPannel::slotPlugOnOff(bool check)
{
    if(check)
    {
        ui->pPBPlug->setChecked(false);
        emit sigSocketSendData("[19]GASON");
    }
    else {
        ui->pPBPlug->setChecked(true);
        emit sigSocketSendData("[19]GASOFF");
    }
}
void Tab3ControlPannel::slotTab3RecvData(QString recvData)
{
//    QMessageBox::information(this,"socket",recvData);  //[19]LAMPON  => @19@LAMPON
    QIcon icon;
    recvData.replace("[","@");
    recvData.replace("]","@");
    QStringList qlist = recvData.split("@");

    qDebug() << qlist[2];
    if(qlist[2] == "LAMPON") {
        ui->pPBLamp->setChecked(true);
        icon.addFile(":/IotQtClient/Image/light_on.png");
    }
    else if(qlist[2] == "LAMPOFF") {
        ui->pPBLamp->setChecked(false);
        icon.addFile(":/IotQtClient/Image/light_off.png");
    }
    else if(qlist[2] == "GASON") {
        ui->pPBPlug->setChecked(true);
    }
    else if(qlist[2] == "GASOFF") {
        ui->pPBPlug->setChecked(false);
    }
}
