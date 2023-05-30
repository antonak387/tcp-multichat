#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>//для сервера
#include <QtNetwork/QTcpSocket>//для соединения
#include <QNetworkInterface>
#include <QMessageBox>
//#include <QRegExpValidator>
#include <QTimer>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>

#include <filtr.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_radioButton_server_clicked();

    void on_radioButton_client_clicked();

    void on_pushButton_create_clicked();

    void new_connection();

    void on_pushButton_connect_clicked();

    void on_pushButton_sendmes_clicked();

    void read_data();

    void on_pushButton_disconnect_clicked();

    void checkconnect();

    void on_lineEdit_sendmes_textChanged();

    void connected();

    void disconnected();

    void on_pushButton_deleteServer_clicked();

    void on_lineEdit_sendmes_spam_textChanged();

    void spam();

    void on_pushButton_sendmes_spam_clicked();

    void on_pushButton_JustCat_clicked();

    void on_pushButton_Cry_clicked();

    void on_pushButton_Laught_clicked();

    void on_pushButton_Poop_clicked();

    void on_pushButton_WrySmile_clicked();

    void on_pushButton_Love_clicked();

    void on_pushButton_Smile_clicked();

    void on_pushButton_show_emoji_clicked();

    void on_pushButton_Look_At_Ip_clicked();

private:
    Ui::MainWindow *ui;

    QVector<QTcpSocket*> sockets;
    QTcpServer *TCPserver;
    QTcpSocket *TCPsocket;
    QTcpSocket *Prsocket;

    QTimer* timer;
    QTimer* timerspam;

    QMediaPlayer * m_player;        // Audio player
    QMediaPlaylist * m_playlist;    // Playlist

        bool emojiIsShow = false;

    bool flagspam;

    bool flcon;
    bool fl;

};

#endif // MAINWINDOW_H
