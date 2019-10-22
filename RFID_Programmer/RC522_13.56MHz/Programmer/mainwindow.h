#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QTimer>
#include "../src/myProtoDefine.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private slots:
	void slot_serialPortReadData();
	void slot_timer();
	void on_connectB_clicked();
	void on_updB_clicked();
	void on_setAddrB_clicked();
	void on_readB_clicked();
	void on_writeB_clicked();
	void on_dumpB_clicked() { sendPkt(CMD_DUMP_DATA,""); }

	void on_setUID_clicked();

private:
	Ui::MainWindow *ui;
	QSerialPort* m_pSerialPort;
	QTimer* m_pTimer;
	QByteArray m_buff;
	Pkt m_sendPkt;
	Pkt m_recvPkt;

	void sendPkt(const uint8_t cmd, const QByteArray &data);
	void reScanComPorts();
	void statusUpd();
	void clearSendPktData() { m_sendPkt.len = 0;for(uint8_t i = 0; i < sizeof(m_sendPkt.data); i++) m_sendPkt.data[i] = 0; }
	void chkPkt(QByteArray &buff);
	void processingPkt();
	QString printHex(const QString &data);
	bool checkPort(const QString &port);
};

#endif // MAINWINDOW_H
