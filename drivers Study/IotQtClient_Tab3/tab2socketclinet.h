#ifndef TAB2SOCKETCLINET_H
#define TAB2SOCKETCLINET_H

#include <QWidget>
#include <QTime>
#include <QDebug>
#include <QMessageBox>
#include "socketclient.h"

namespace Ui {
class Tab2SocketClinet;
}

class Tab2SocketClinet : public QWidget
{
    Q_OBJECT

public:
    explicit Tab2SocketClinet(QWidget *parent = nullptr);
    ~Tab2SocketClinet();

private:
    Ui::Tab2SocketClinet *ui;
    SocketClient* pSocketClient;
private slots:
    void slotConnectToServer(bool);
    void slotSocketRecvUpdate(QString);
    void slotSocketSendData();
    void slotSocketSendData(QString);
signals:
    void sigSocketRecv(QString);
    void sigTab3RecvData(QString);
};

#endif // TAB2SOCKETCLINET_H
