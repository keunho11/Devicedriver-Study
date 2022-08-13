#include "tab2socketclinet.h"
#include "ui_tab2socketclinet.h"

Tab2SocketClinet::Tab2SocketClinet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab2SocketClinet)
{
    ui->setupUi(this);
    pSocketClient = new SocketClient(this);
    ui->pPBSendButton->setEnabled(false);
    connect(ui->pPBServerConnect,SIGNAL(clicked(bool)),this,SLOT(slotConnectToServer(bool)));
    connect(ui->pPBSendButton,SIGNAL(clicked()),this,SLOT(slotSocketSendData()));
    connect(pSocketClient,SIGNAL(sigSocketRecv(QString)),this,SLOT(slotSocketRecvUpdate(QString)));
    connect(ui->pPBRecvDataClear,SIGNAL(clicked()),ui->pTERecvData, SLOT(clear()));
}

Tab2SocketClinet::~Tab2SocketClinet()
{
    delete ui;
}

void Tab2SocketClinet::slotConnectToServer(bool check)
{
    bool bOk;
    if(check) {
        pSocketClient->slotConnectToServer(bOk);
        if(bOk) {
            ui->pPBServerConnect->setText("서버 해제");
            ui->pPBSendButton->setEnabled(true);
        }
    }
    else {
        pSocketClient->slotClosedByServer();
        ui->pPBServerConnect->setText("서버 연결");
        ui->pPBSendButton->setEnabled(false);
    }
}
void Tab2SocketClinet::slotSocketRecvUpdate(QString strRecvData)
{
    QTime time = QTime::currentTime();
    QString strTime = time.toString();
//    qDebug() << strTime;
    strRecvData.chop(1);    //'\n' 제거
    if((strRecvData.indexOf("LAMP") != -1) || (strRecvData.indexOf("GAS") != -1))
    {
        emit sigTab3RecvData(strRecvData);
    }

    strRecvData = strTime + " " + strRecvData;
    ui->pTERecvData->append(strRecvData);
    ui->pTERecvData->moveCursor(QTextCursor::End);
}

void Tab2SocketClinet::slotSocketSendData()
{
     QString strRecvId;
     QString strSendData;

     strRecvId = ui->pLERecvId->text();
     strSendData = ui->pLESendData->text();
     if(strRecvId.isEmpty())
         strSendData = "[ALLMSG]"+strSendData;
     else
         strSendData = "["+strRecvId+"]"+strSendData;
     pSocketClient->slotSocketSendData(strSendData);
     ui->pLESendData->clear();
}
void Tab2SocketClinet::slotSocketSendData(QString strData)
{
    if(ui->pPBServerConnect->isChecked())
        pSocketClient->slotSocketSendData(strData);
    else
        QMessageBox::information(this,"socket","서버 연결 확인");
}
