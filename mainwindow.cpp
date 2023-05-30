#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)//конструктор
{



    ui->setupUi(this);

    TCPsocket = new QTcpSocket(this);
    TCPserver= new QTcpServer(this);
    Prsocket = new QTcpSocket(this);

    timer = new QTimer(this);
    timerspam = new QTimer(this);
    fl=false;
    flagspam=false;
    connect(timer,SIGNAL(timeout()),this,SLOT(checkconnect()));//соединили сигнал таймера с обработчиком
    connect(timerspam,SIGNAL(timeout()),this,SLOT(spam()));//соединили сигнал таймера с обработчиком

    connect(TCPserver,SIGNAL(newConnection()),this, SLOT(new_connection()));

    //connect(TCPsocket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    //connect(TCPsocket, SIGNAL(connected()),this, SLOT(connected()));

    ui->textEdit_My_Ip->setReadOnly(true);
    ui->pushButton_disconnect->setEnabled(false);
    ui->pushButton_create->hide();
    ui->pushButton_connect->hide();
    ui->pushButton_sendmes->hide();
    ui->label_status->hide();
    ui->pushButton_disconnect->hide();
    ui->lineEdit_sendmes->hide();
    ui->lineEdit_sendmes_spam->hide();
    ui->pushButton_sendmes_spam->hide();
    ui->label_nickname->hide();
    ui->lineEdit_nickname->hide();
    ui->pushButton_deleteServer->hide();
    ui->lineEdit_port->hide();
    ui->lineEdit_adress->hide();
    ui->label_port->hide();
    ui->label_adress->hide();
    ui->textEdit_chat->hide();
    ui->label_status->setText("<font color=red>unconnected</font>");

    ui->groupBox_emoji->setVisible(false);
    ui->pushButton_show_emoji->setVisible(false);


    ui->groupBox_emoji->setVisible(false);

    m_player = new QMediaPlayer(this);          // Init player
    m_playlist = new QMediaPlaylist(m_player);  // Init playlist
    m_player->setPlaylist(m_playlist);          // Set playlist into player
    m_playlist->addMedia(QUrl("qrc:/beeb.wav"));       // Adding of track to playlist



    //проверяем введенный адрес получателя
    //QRegExp regExp("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    //ui->lineEdit_adress->setValidator(new QRegExpValidator(regExp,this));
    //выводим наш IP


    //    QList<QHostAddress> list= QNetworkInterface::allAddresses();
    //    for(int i=0; i<list.count(); i++)
    //    {
    //        if(!list[i].isLoopback())
    //        {
    //            //if (list[i].protocol() == QAbstractSocket::IPv4Protocol)
    //            //{
    //            //if (regExp.exactMatch(list[i].toString()))
    //            //{
    //            //ui->comboBox_ourIp->addItem(list[i].toString());
    //            //}
    //            //}
    //        }
    //    }


}

MainWindow::~MainWindow()//деструктор
{
    if (ui->radioButton_client->isChecked() and fl)
    {
        TCPsocket->write((ui->lineEdit_nickname->text()+" отключился").toUtf8());
        TCPsocket->close();
        TCPsocket->waitForDisconnected(50);
    }
    for(auto ol : sockets)
    {
        ol->abort();
        //sockets.removeOne(ol);
        //delete ol;
    }
    delete ui;
    sockets.clear();
    sockets.squeeze();
    Prsocket->abort();
    //delete TCPsocket;
    delete Prsocket;
    delete TCPserver;
    timer->stop();
    delete timer;
    timerspam->stop();
    delete timerspam;



}

void MainWindow::connected()//слот подключения
{
    ui->label_status->setText("<font color=green>connected</font>");
    ui->pushButton_connect->setEnabled(false);
    ui->pushButton_disconnect->setEnabled(true);
    ui->lineEdit_port->setReadOnly(true);
    ui->lineEdit_adress->setReadOnly(true);
    ui->lineEdit_nickname->setReadOnly(true);
    fl=true;


    filtr myfiltr;

    QString myNick = ui->lineEdit_nickname->text();
    myNick = myfiltr.mat_filtr(myNick);


    TCPsocket->write((myNick+" присоединился к чату").toUtf8());
    on_lineEdit_sendmes_textChanged();
    on_lineEdit_sendmes_spam_textChanged();
}

void MainWindow::disconnected()//слот отключения
{
    ui->label_status->setText("<font color=red>unconnected</font>");
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_disconnect->setEnabled(false);
    ui->textEdit_chat->hide();
    ui->lineEdit_sendmes->hide();
    ui->lineEdit_sendmes_spam->hide();
    ui->pushButton_sendmes_spam->hide();
    ui->pushButton_sendmes->hide();

    ui->pushButton_show_emoji->setVisible(false);
    ui->groupBox_emoji->setVisible(false);


    ui->lineEdit_port->setReadOnly(false);
    ui->lineEdit_adress->setReadOnly(false);
    ui->lineEdit_nickname->setReadOnly(false);
    TCPsocket->abort();
    //timer->stop();
    timerspam->stop();
    flagspam=false;
    fl=false;
    ui->textEdit_chat->clear();
    ui->groupBox->setEnabled(true);
    on_lineEdit_sendmes_textChanged();
    on_lineEdit_sendmes_spam_textChanged();
}
void MainWindow::new_connection()//обработка нового подключения
{
    TCPsocket = TCPserver->nextPendingConnection();
    timer->start(10);
    sockets.push_back(TCPsocket);
    connect(TCPsocket,SIGNAL(readyRead()),this,SLOT(read_data()));
    ui->label_status->setText("<font color=green>connected</font>");
    ui->textEdit_chat->show();
    ui->lineEdit_sendmes->show();
    ui->lineEdit_sendmes_spam->show();
    ui->pushButton_sendmes_spam->show();
    ui->pushButton_sendmes->show();

    ui->pushButton_show_emoji->setVisible(true);

    fl=true;
    on_lineEdit_sendmes_textChanged();
    on_lineEdit_sendmes_spam_textChanged();

}

void MainWindow::read_data()//пришли данные
{

    m_player->stop();
    m_player->play();
    QByteArray ba;
    QByteArray proverka;
    if (ui->radioButton_client->isChecked())
    {
        ba = TCPsocket->readAll();

        QString stroka(ba);
        if (!stroka.isEmpty()) //and stroka!=(ui->lineEdit_nickname->text() + ": " + ui->lineEdit_sendmes->text()))
        {
            ui->textEdit_chat->append(stroka);

        }

    }


    if (ui->radioButton_server->isChecked())
    {
        for(auto ol : sockets)
        {
            ba = ol->readAll();
            QString stroka(ba);
            if (stroka!="")
            {
                ui->textEdit_chat->append(stroka);
                proverka=ba;
                Prsocket=ol;
            }
        }
        for(auto el : sockets)
        {
            if(el != Prsocket)
                el->write(proverka);
        }

    }

}

void MainWindow::on_radioButton_server_clicked()//выбран сервер
{
    ui->pushButton_create->show();
    ui->pushButton_connect->hide();
    ui->pushButton_sendmes->hide();
    ui->pushButton_disconnect->hide();

    ui->label_port->show();
    ui->label_nickname->show();
    ui->label_adress->hide();
    TCPsocket->abort();
    ui->lineEdit_nickname->show();
    ui->lineEdit_port->show();
    ui->lineEdit_adress->hide();
    ui->lineEdit_sendmes->hide();
    ui->lineEdit_sendmes_spam->hide();
    ui->pushButton_sendmes_spam->hide();

    ui->pushButton_show_emoji->setVisible(false);
    ui->groupBox_emoji->setVisible(false);

    ui->textEdit_chat->clear();

    ui->textEdit_chat->hide();
    QPoint point;
    point = ui->lineEdit_nickname->pos();
    point.setY(150);
    ui->lineEdit_nickname->move(point);

    point = ui->pushButton_create->pos();
    point.setY(190);
    ui->pushButton_create->move(point);

    point = ui->label_nickname->pos();
    point.setY(120);
    ui->label_nickname->move(point);

//    point = ui->label_status->pos();
//    point.setY(390);
//    ui->label_status->move(point);

    point = ui->pushButton_deleteServer->pos();
    point.setY(240);
    ui->pushButton_deleteServer->move(point);

}

void MainWindow::on_radioButton_client_clicked()//выбран клиент
{
    ui->pushButton_create->hide();
    ui->pushButton_connect->show();
    ui->pushButton_sendmes->hide();
    ui->pushButton_disconnect->show();
    ui->pushButton_deleteServer->hide();
    ui->textEdit_chat->clear();
    ui->label_port->show();
    ui->label_nickname->show();
    ui->label_adress->show();
    TCPsocket->abort();
    ui->lineEdit_nickname->show();
    ui->lineEdit_port->show();
    ui->lineEdit_adress->show();
    ui->lineEdit_sendmes->hide();
    ui->lineEdit_sendmes_spam->hide();
    ui->pushButton_sendmes_spam->hide();

    ui->pushButton_show_emoji->setVisible(false);
    ui->groupBox_emoji->setVisible(false);

    ui->textEdit_chat->hide();

    ui->textEdit_chat->hide();
    QPoint point;
    point = ui->lineEdit_nickname->pos();
    point.setY(190);
    ui->lineEdit_nickname->move(point);

    point = ui->pushButton_disconnect->pos();
    point.setY(280);
    ui->pushButton_disconnect->move(point);

    point = ui->label_nickname->pos();
    point.setY(160);
    ui->label_nickname->move(point);

//    point = ui->label_status->pos();
//    point.setY(430);
//    ui->label_status->move(point);

    point = ui->pushButton_connect->pos();
    point.setY(230);
    ui->pushButton_connect->move(point);

}

void MainWindow::on_pushButton_create_clicked()//кнопка создать
{
    int port;
    bool flag;
    port = ui->lineEdit_port->text().toInt(&flag);
    if (flag and port>0 and !ui->lineEdit_port->displayText().isEmpty() and port<=65535)
    {
        if (!ui->lineEdit_nickname->displayText().isEmpty())
        {
            TCPserver->close();
            TCPserver->listen(QHostAddress::Any, quint16(port));
            if (TCPserver->isListening())
            {

                ui->textEdit_chat->clear();
                ui->groupBox->setEnabled(false);
                ui->pushButton_create->setEnabled(false);
                ui->textEdit_chat->show();

                ui->lineEdit_sendmes->show();
                ui->lineEdit_sendmes_spam->show();
                ui->pushButton_sendmes_spam->show();

                ui->pushButton_sendmes->show();

                ui->pushButton_show_emoji->setVisible(true);

                ui->label_status->show();
                //ui->pushButton_disconnect->show();
                //ui->lineEdit_port->setReadOnly(true);
                ui->pushButton_deleteServer->show();
                ui->pushButton_deleteServer->setEnabled(true);
                ui->lineEdit_port->setReadOnly(true);
                ui->lineEdit_adress->setReadOnly(true);
                ui->lineEdit_nickname->setReadOnly(true);
                on_lineEdit_sendmes_textChanged();
                on_lineEdit_sendmes_spam_textChanged();
            }

            else
            {
                QMessageBox::warning(this,"Ошибка", "Такой сервер уже создан",QMessageBox::Ok);
            }
        }
        else
        {
            QMessageBox::warning(this,"Ошибка", "Введите никнейм",QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this,"Ошибка", "Неправильно введены данные",QMessageBox::Ok);
    }
}

void MainWindow::on_pushButton_connect_clicked()//кнопка подключения
{
    bool flag;
    disconnect(TCPsocket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    disconnect(TCPsocket, SIGNAL(connected()),this, SLOT(connected()));
    connect(TCPsocket, SIGNAL(disconnected()),this, SLOT(disconnected()));
    connect(TCPsocket, SIGNAL(connected()),this, SLOT(connected()));
    //QRegExp regExp("(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)");
    QHostAddress * adress;
    adress = new QHostAddress;
    // (regExp.exactMatch(ui->lineEdit_adress->text()))
    //{
    adress->setAddress(ui->lineEdit_adress->text());
    int port;
    port = ui->lineEdit_port->text().toInt(&flag);
    if (flag and port>0 and !ui->lineEdit_port->displayText().isEmpty() and port<=65535)
    {
        if (!ui->lineEdit_nickname->displayText().isEmpty())
        {
            TCPsocket->connectToHost(*adress,quint16(port));
            TCPsocket->waitForConnected(1000);
            if (TCPsocket->state() == QAbstractSocket::UnconnectedState)
            {
                TCPsocket->abort();
                QMessageBox::warning(this,"Ошибка", "Сервер не существует",QMessageBox::Ok);
                ui->groupBox->setEnabled(true);
                ui->lineEdit_port->setReadOnly(false);
                ui->lineEdit_adress->setReadOnly(false);
                ui->lineEdit_nickname->setReadOnly(false);

            }
            else
            {
                connect(TCPsocket,SIGNAL(readyRead()),this,SLOT(read_data()));
                checkconnect();
                ui->textEdit_chat->show();
                ui->lineEdit_sendmes->show();
                ui->pushButton_sendmes->show();
                ui->lineEdit_sendmes_spam->show();
                ui->pushButton_sendmes_spam->show();

                ui->pushButton_show_emoji->setVisible(true);




                ui->label_status->show();
                ui->pushButton_disconnect->show();
                //fl=true;

                ui->groupBox->setEnabled(false);
            }
        }
        else
        {
            QMessageBox::warning(this,"Ошибка", "Неправильно введены данные",QMessageBox::Ok);
        }




        //                QPoint point;
        //                point = ui->pushButton_disconnect->pos();
        //                point.setY(410);
        //                ui->pushButton_disconnect->move(point);

        //                point = ui->label_status->pos();
        //                point.setY(460);
        //                ui->label_status->move(point);




    }
    else
    {
        QMessageBox::warning(this,"Ошибка", "Неправильно введены данные",QMessageBox::Ok);
    }
    //}
    //else
    //{
    //   QMessageBox::warning(this,"Ошибка", "Неправильно введены данные",QMessageBox::Ok);
    //}
    delete adress;

}

void MainWindow::on_pushButton_sendmes_clicked()//кнопка отправки сообщения
{
    if (ui->radioButton_client->isChecked())
    {

        filtr myfiltr;

        QString myMessege = ui->lineEdit_sendmes->text();
        QString myNick = ui->lineEdit_nickname->text();

        myMessege = myfiltr.mat_filtr(myMessege);
        myNick = myfiltr.mat_filtr(myNick);


        QString baq= myNick +": " + myMessege;

        QByteArray ba =baq.toUtf8();
        TCPsocket->write(ba);
        ui->textEdit_chat->append(baq);
    }
    if (ui->radioButton_server->isChecked())
    {
        filtr myfiltr;

        QString myMessege = ui->lineEdit_sendmes->text();
        QString myNick = ui->lineEdit_nickname->text();

        myMessege = myfiltr.mat_filtr(myMessege);
        myNick = myfiltr.mat_filtr(myNick);


        QString baq= myNick + ": " + myMessege;

        for(auto el : sockets)
        {
            QByteArray ba =baq.toUtf8();
            el->write(ba);
        }
        ui->textEdit_chat->append(baq);

    }
    ui->lineEdit_sendmes->clear();

}

void MainWindow::on_pushButton_disconnect_clicked()//кнопка отключения от сервера
{
    if (flagspam)
    {
        timerspam->stop();
        ui->pushButton_sendmes_spam->setText("Спам");
        flagspam = false;
    }

    filtr myfiltr;

    QString myMessege = ui->lineEdit_sendmes->text();
    QString myNick = ui->lineEdit_nickname->text();

    TCPsocket->write((myNick +" отключился").toUtf8());

    TCPsocket->waitForDisconnected(5);
    TCPsocket->abort();
}

void MainWindow::checkconnect()//есть коннект
{
    if (ui->radioButton_server->isChecked())
    {
        flcon = false;
        for(auto el : sockets)
        {
            fl=true;
            if (el->state()==QAbstractSocket::UnconnectedState)
            {
                sockets.removeOne(el);
                flcon = true;

            }
            else {
                ui->label_status->setText("<font color=green>connected</font>");
                ui->pushButton_deleteServer->setEnabled(true);
                flcon = true;
                //ui->pushButton_create->setEnabled(true);

            }
        }
        if (!flcon)
        {
            ui->label_status->setText("<font color=red>unconnected</font>");
            fl=false;
            timerspam->stop();
            flagspam=false;
            on_lineEdit_sendmes_textChanged();
            on_lineEdit_sendmes_spam_textChanged();
            timer->stop();
        }
    }
}

void MainWindow::on_lineEdit_sendmes_textChanged()//включение/отключение возможности отправить сообщение
{
    if (!ui->lineEdit_sendmes->displayText().isEmpty() and fl)
    {
        ui->pushButton_sendmes->setEnabled(true);

    }
    else
    {
        ui->pushButton_sendmes->setEnabled(false);

    }
}

void MainWindow::on_pushButton_deleteServer_clicked()//кнопка удаления сервера
{
    if (flagspam)
    {
        timerspam->stop();
        ui->pushButton_sendmes_spam->setText("Спам");
        flagspam = false;
    }

    timer->stop();
    fl=false;

    TCPsocket->abort();
    TCPserver->close();

    for(auto el : sockets)
    {

        //sockets.removeAll(el);
        el->abort();
    }

    ui->groupBox->setEnabled(true);
    ui->lineEdit_port->setReadOnly(false);
    ui->lineEdit_adress->setReadOnly(false);
    ui->lineEdit_nickname->setReadOnly(false);
    ui->pushButton_create->setEnabled(true);
    ui->pushButton_deleteServer->setEnabled(false);
    ui->lineEdit_sendmes->hide();
    ui->lineEdit_sendmes_spam->hide();
    ui->pushButton_sendmes_spam->hide();
    ui->textEdit_chat->hide();
    ui->pushButton_sendmes->hide();

    ui->pushButton_show_emoji->setVisible(false);
    ui->groupBox_emoji->setVisible(false);

    ui->label_status->setText("<font color=red>unconnected</font>");
}

void MainWindow::on_lineEdit_sendmes_spam_textChanged()//включениеотключение возможности спамить
{
    if (!ui->lineEdit_sendmes_spam->displayText().isEmpty() and fl)
    {
        ui->pushButton_sendmes_spam->setEnabled(true);

    }
    else
    {
        ui->pushButton_sendmes_spam->setEnabled(false);

    }
}

void MainWindow::spam()//спамим
{
    checkconnect();
    if (ui->radioButton_client->isChecked())
    {
        if (ui->lineEdit_sendmes_spam->displayText().isEmpty() or !fl)
        {
            timerspam->stop();
            flagspam = false;
        }
        else
        {

            filtr myfiltr;

            QString myMessege = ui->lineEdit_sendmes_spam->text();
            QString myNick = ui->lineEdit_nickname->text();

            myMessege = myfiltr.mat_filtr(myMessege);
            myNick = myfiltr.mat_filtr(myNick);


            QString baq = myNick + ": " + myMessege;


            QByteArray ba =baq.toUtf8();
            TCPsocket->write(ba);


            if (!baq.isEmpty())
            {
                ui->textEdit_chat->append(baq);
            }
        }
    }
    if (ui->radioButton_server->isChecked())
    {
        if (ui->lineEdit_sendmes_spam->displayText().isEmpty() or !fl)
        {
            timerspam->stop();
            flagspam = false;
        }
        else
        {
            filtr myfiltr;

            QString myNick = ui->lineEdit_nickname->text();
            myNick = myfiltr.mat_filtr(myNick);


            QString myMessege = ui->lineEdit_sendmes_spam->text();


            myMessege = myfiltr.mat_filtr(myMessege);



            QString baq= myNick + ": " + myMessege;

            for(auto el : sockets)
            {
                QByteArray ba =baq.toUtf8();
                el->write(ba);
            }
            if (!baq.isEmpty())
            {
                ui->textEdit_chat->append(baq);
            }

        }
    }
}

void MainWindow::on_pushButton_sendmes_spam_clicked()//нажали на кнопку спама
{
    if (!flagspam)
    {
        timerspam->start(600);
        ui->pushButton_sendmes_spam->setText("Стоп");
        flagspam = true;
    }
    else
    {
        timerspam->stop();
        ui->pushButton_sendmes_spam->setText("Спам");
        flagspam = false;
    }
}

void MainWindow::on_pushButton_Poop_clicked()// нажать на смайлик
{
    ui->lineEdit_sendmes->insert("💩");
}

void MainWindow::on_pushButton_Smile_clicked()//нажать на смайлик
{
    ui->lineEdit_sendmes->insert("😺");
}
void MainWindow::on_pushButton_Laught_clicked()//нажать на смайлик
{
    ui->lineEdit_sendmes->insert("😹");
}
void MainWindow::on_pushButton_Love_clicked()//нажать на смайлик
{
    ui->lineEdit_sendmes->insert("😻");
}
void MainWindow::on_pushButton_Cry_clicked()//нажать на смайлик
{
    ui->lineEdit_sendmes->insert("😿");
}
void MainWindow::on_pushButton_WrySmile_clicked()//нажать на смайлик
{
    ui->lineEdit_sendmes->insert("😼");
}
void MainWindow::on_pushButton_JustCat_clicked()//нажать на смайлик
{
    ui->lineEdit_sendmes->insert("🐈");
}


void MainWindow::on_pushButton_show_emoji_clicked()//показать все смайлики
{
    if(!emojiIsShow)
    {
        ui->groupBox_emoji->setVisible(true);
        emojiIsShow = true;
    }
    else
    {
        ui->groupBox_emoji->setVisible(false);
        emojiIsShow = false;
    }
}

void MainWindow::on_pushButton_Look_At_Ip_clicked()//посмотреть ip
{


    ui->textEdit_My_Ip->clear();
    QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
    QNetworkInterface eth;

    foreach(eth, allInterfaces) {
        QList<QNetworkAddressEntry> allEntries = eth.addressEntries();
        QNetworkAddressEntry entry;
        foreach (entry, allEntries)
        {
            if(entry.ip().toString()!="::1"&&entry.ip().toString()!="127.0.0.1")
                ui->textEdit_My_Ip->append(entry.ip().toString());
        }
    }
}
