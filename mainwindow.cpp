#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "winsock2.h"
#include "iphlpapi.h"
#include "icmpapi.h"
#include <QRegExpValidator>

/*
Создать приложение, которое эмулирует поведение программы
ping в рамках посылки цепочки ECHO-запросов по указанному
адресу и получению ECHO-ответов на них с замером времени.
Вывод на экран должен быть аналогичен ping
*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

        HANDLE hIcmpFile;
        unsigned long ipaddr = INADDR_NONE;
        DWORD dwRetVal = 0;
        char SendData[32] = "Data Buffer";
        LPVOID ReplyBuffer = NULL;
        DWORD ReplySize = 0;


        ipaddr = inet_addr(ui->lineEdit->text().toStdString().c_str());
        hIcmpFile = IcmpCreateFile();


        ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
        ReplyBuffer = (VOID*) malloc(ReplySize);


        dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
                    NULL, ReplyBuffer, ReplySize, 1000);


        QString strMessage = "";

        if (dwRetVal != 0) {

            PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
            struct in_addr ReplyAddr;
            ReplyAddr.S_un.S_addr = pEchoReply->Address;

            strMessage += "Sent icmp message to " + ui->lineEdit->text() + "\n";
            if (dwRetVal > 1) {
                strMessage += "Received " + QString::number(dwRetVal) + " icmp message responses \n";
                strMessage += "Information from the first response: ";
            }
            else {
                strMessage += "Received " + QString::number(dwRetVal) + " icmp message response \n";
                strMessage += "Information from the first response: ";
            }
                strMessage += "Received from ";
                strMessage += inet_ntoa( ReplyAddr );
                strMessage += "\n";
                strMessage += "Status = " + pEchoReply->Status;
                strMessage += "Roundtrip time = " + QString::number(pEchoReply->RoundTripTime) + " milliseconds \n";
        } else {
            strMessage += "Call to IcmpSendEcho failed.\n";
            strMessage += "IcmpSendEcho returned error: ";
            strMessage += QString::number(GetLastError());
        }

        ui->textBrowser->setText(strMessage);
        free(ReplyBuffer);
}
