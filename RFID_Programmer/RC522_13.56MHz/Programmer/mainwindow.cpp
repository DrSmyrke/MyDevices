#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDir>
#include <QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setWindowIcon(QIcon("://index.ico"));

	m_pSerialPort = new QSerialPort(this);
		connect(m_pSerialPort,&QSerialPort::readyRead,this,&MainWindow::slot_serialPortReadData);
	m_pTimer=new QTimer();
		m_pTimer->setInterval(250);
		connect(m_pTimer,&QTimer::timeout,this,&MainWindow::slot_timer);
		m_pTimer->start();

	statusUpd();
	reScanComPorts();

	ui->blockDataBox->setInputMask(">HH HH HH HH HH HH HH HH HH HH HH HH HH HH HH HH;_");
	ui->uidBox->setInputMask(">HH HH HH HH;_");
}

MainWindow::~MainWindow()
{
	if(m_pSerialPort->isOpen()) m_pSerialPort->close();
	delete ui;
}

void MainWindow::slot_serialPortReadData()
{
	m_pSerialPort->waitForReadyRead(100);
	while(m_pSerialPort->bytesAvailable()){
		QByteArray data = m_pSerialPort->read(1);
		m_buff.append(data);
	}

	if(static_cast<uint8_t>( m_buff.at(0) ) != START_BYTE ){
		QByteArray buff;
		while( m_buff.contains( '\r' ) ){
			auto sym = m_buff.left( 1 );
			m_buff.remove( 0, 1 );

			if( sym.at( 0 ) == '\n' ) continue;
			if( sym.at( 0 ) == '\r' ){
				ui->logBox->insertPlainText( QString( buff ) );
				buff.clear();
				continue;
			}
			buff.append( sym );
		}

		auto sym = m_buff.left( 1 );
		if( sym.at( 0 ) == '\n' ){
			m_buff.remove( 0, 1 );
		}
	}


	while( m_buff.size() > 4 ) chkPkt(m_buff);
}

void MainWindow::slot_timer()
{
	if(m_pSerialPort->bytesAvailable()) slot_serialPortReadData();

	if( !m_recvPkt.forProcessing && m_buff.size() > 0 ) chkPkt(m_buff);
}

void MainWindow::reScanComPorts()
{
	ui->portSelector->clear();
	for( auto portInfo:QSerialPortInfo::availablePorts() ){
		ui->portSelector->addItem( portInfo.portName() );
	}
//	ui->portSelector->clear();
//	QStringList devs;
//	#if defined(Q_OS_WIN)
//		for( uint8_t i = 1; i < 250; i++ ){
//			auto str = QString("COM%1").arg( i );
//			if( checkPort( str ) ) ui->portSelector->addItem( str );
//		}
//	#elif defined(Q_OS_UNIX)
//		QDir dir=QDir("/dev");
//		devs=dir.entryList(QStringList() << "ttyUSB*",QDir::NoDotAndDotDot | QDir::System);
//		ui->portSelector->addItems(devs);
//		devs=dir.entryList(QStringList() << "ttyACM*",QDir::NoDotAndDotDot | QDir::System);
//		ui->portSelector->addItems(devs);
//	#endif
}

void MainWindow::statusUpd()
{
	if(m_pSerialPort->isOpen()){
		ui->statusLabel->setText("<span style=\"color:green\">["+tr("connected")+"]</span>");
		ui->connectB->setText(tr("DISCONNECT"));
	}else{
		ui->statusLabel->setText("<span style=\"color:red\">["+tr("disconnected")+"]</span>");
		ui->connectB->setText(tr("CONNECT"));
		reScanComPorts();
		ui->widgetBox->setEnabled(false);
	}
}

void MainWindow::chkPkt(QByteArray &buff)
{
	if( buff.size() < 5 ) return;
	if( static_cast<uint8_t>( buff.at(0) ) != START_BYTE ){
		buff.remove( 0, 1 );
		return;
	}

	m_recvPkt.cmd = static_cast<uint8_t>( buff[1] );
	m_recvPkt.len = static_cast<uint8_t>( buff[2] );

	if( buff.size() < 5 + m_recvPkt.len ) return;

	uint8_t i = 3;
	uint8_t crc = 0;
	for(i = 3; i < m_recvPkt.len + 3; i++){
		m_recvPkt.data[i-3] = static_cast<uint8_t>( buff[i] );
		crc += m_recvPkt.data[i-3];
	}

	m_recvPkt.crc = static_cast<uint8_t>( buff[i] );
	if( static_cast<uint8_t>( buff.at(++i) ) != STOP_BYTE ) return;

	crc += m_recvPkt.cmd;
	crc += m_recvPkt.len;

	if( crc != m_recvPkt.crc ) return;

	//success parsing
	m_recvPkt.forProcessing = true;
	processingPkt();
	ui->logBox->insertPlainText( printHex(buff.left(5 + m_recvPkt.len).toHex()) );
	buff.remove( 0, 5 + m_recvPkt.len );
}

void MainWindow::processingPkt()
{
	bool flag = false;
	QByteArray miniBuff;
	switch (m_recvPkt.cmd) {
		case CMD_READ_VERSION:
			if(m_recvPkt.len == 1){
				flag = false;
				switch (m_recvPkt.data[0]) {
					case 0x00: ui->versionLabel->setText("<span style=\"color:red\"><b>ERROR</b></span>"); break;
					case 0xFF: ui->versionLabel->setText("<span style=\"color:red\"><b>ERROR</b></span>"); break;
					case 0x91: ui->versionLabel->setText("v1.0");flag = true; break;
					case 0x92: ui->versionLabel->setText("v2.0");flag = true; break;
					default: ui->versionLabel->setText("unknown"); break;
				}
				//Если версия железа поддерживается, разрешаем работу
				if( flag ) ui->widgetBox->setEnabled(true);
			}else{
				//TODO: реализовать отправку ошибки для повторной отправки пакета
			}
		break;
		case CMD_READ_UID:
			if(m_recvPkt.len > 0){
				miniBuff.clear();
				for(uint8_t i = 0; i < m_recvPkt.len; i++) miniBuff.append( static_cast< char >( m_recvPkt.data[i] ) );
				ui->uidBox->setText( miniBuff.toHex() );
			}else{
				//TODO: реализовать отправку ошибки для повторной отправки пакета
			}
		break;
		case CMD_READ_PICC_TYPE:
			if(m_recvPkt.len == 1){
				switch (m_recvPkt.data[0]) {
					case 0x00: ui->typeLabel->setText("<span style=\"color:red\"><b>unknown</b></span>"); break;
					case 0x01: ui->typeLabel->setText("ISO/IEC 14443-4"); break;
					case 0x02: ui->typeLabel->setText("ISO/IEC 18092 (NFC)"); break;
					case 0x03: ui->typeLabel->setText("MIFARE_MINI [320 bytes]"); break;
					case 0x04: ui->typeLabel->setText("MIFARE_1K [1KB]"); break;
					case 0x05: ui->typeLabel->setText("MIFARE_4K [4KB]"); break;
					case 0x06: ui->typeLabel->setText("MIFARE Ultralight"); break;
					case 0x07: ui->typeLabel->setText("MIFARE Plus"); break;
					case 0x08: ui->typeLabel->setText("MIFARE TNP3XXX"); break;
					default: ui->typeLabel->setText("<span style=\"color:red\"><b>SAK indicates UID is not complete</b></span>"); break;
				}
			}else{
				//TODO: реализовать отправку ошибки для повторной отправки пакета
			}
		break;
		case CMD_SET_ADDRESS:
			if(m_recvPkt.len > 0){
				ui->blockAddrBox->setValue(m_recvPkt.data[0]);
			}else{
				//TODO: реализовать отправку ошибки для повторной отправки пакета
			}
		break;
		case CMD_READ_BLOCK:
			if(m_recvPkt.len > 0){
				miniBuff.clear();
				for(uint8_t i = 0; i < m_recvPkt.len; i++) miniBuff.append( static_cast< char >( m_recvPkt.data[i] ) );
				ui->blockDataBox->setText( miniBuff.toHex() );
			}else{
				//TODO: реализовать отправку ошибки для повторной отправки пакета
			}
		break;
		case CMD_ERROR:
			if(m_recvPkt.len > 0){
				miniBuff.clear();
				for(uint8_t i = 0; i < m_recvPkt.len; i++) miniBuff.append( static_cast< char >( m_recvPkt.data[i] ) );
				ui->statusLabel_2->setText("<span style=\"color:red\"><b>" + QString(miniBuff) + "</b></span>");
			}else{
				//TODO: реализовать отправку ошибки для повторной отправки пакета
			}
		break;
		case CMD_SUCCESS:
			if(m_recvPkt.len > 0){
				miniBuff.clear();
				for(uint8_t i = 0; i < m_recvPkt.len; i++) miniBuff.append( static_cast< char >( m_recvPkt.data[i] ) );
				ui->statusLabel_2->setText("<span style=\"color:green\"><b>" + QString(miniBuff) + "</b></span>");
			}else{
				//TODO: реализовать отправку ошибки для повторной отправки пакета
			}
		break;
		case CMD_DUMP_DATA:
			if(m_recvPkt.len > 0){
				ui->tabWidget->setCurrentIndex(1);
				miniBuff.clear();
				for(uint8_t i = 0; i < m_recvPkt.len - 1; i++) miniBuff.append( static_cast< char >( m_recvPkt.data[i] ) );
				if( m_recvPkt.len > 17 ){
					ui->dumpDataBox->append( "# " + QString::number( m_recvPkt.data[m_recvPkt.len - 1] ) + "	" + miniBuff );
				}else{
					ui->dumpDataBox->append( "# " + QString::number( m_recvPkt.data[m_recvPkt.len - 1] ) + "	" + printHex( miniBuff.toHex().toUpper() ) );
				}
			}else{
				//TODO: реализовать отправку ошибки для повторной отправки пакета
			}
		break;
	}
	m_recvPkt.forProcessing = false;
}

QString MainWindow::printHex(const QString &data)
{
	QString str;
	for(int i = 0; i < data.size(); i++){
		str += data.at(i);
		if( i%2 ) str += " ";
	}
	if( str.right(1) == " " ) str.remove(str.length()-1,1);
	return str;
}

bool MainWindow::checkPort(const QString &port)
{
	bool res = false;

	if( m_pSerialPort->isOpen() ) m_pSerialPort->close();

	m_pSerialPort->setPortName( port );

	if( m_pSerialPort->open( QIODevice::ReadOnly ) ){
		m_pSerialPort->close();
		res = true;
	}

	return res;
}

void MainWindow::on_connectB_clicked()
{
	if(m_pSerialPort->isOpen()){
		m_pSerialPort->close();
	}else{
		m_pSerialPort->setPortName(ui->portSelector->currentText());
		m_pSerialPort->setBaudRate(9600);
		m_pSerialPort->setDataBits(QSerialPort::Data8);
		m_pSerialPort->setParity(QSerialPort::NoParity);
		m_pSerialPort->setStopBits(QSerialPort::OneStop);
		m_pSerialPort->setFlowControl(QSerialPort::NoFlowControl);
		m_pSerialPort->open(QIODevice::ReadWrite);

		QTimer::singleShot(3000,this,[this](){ sendPkt(CMD_READ_VERSION,""); });
	}
	statusUpd();
}

void MainWindow::on_updB_clicked()
{
	ui->typeLabel->setText(tr("Updating..."));
	sendPkt(CMD_READ_UID_AND_TYPE,"");
}

void MainWindow::sendPkt(const uint8_t cmd, const QByteArray &data)
{
	if( !m_pSerialPort->isOpen() ) return;

	ui->statusLabel_2->clear();
	clearSendPktData();
	m_sendPkt.cmd = cmd;
	m_sendPkt.len = static_cast< uint8_t >( data.size() );
	m_sendPkt.crc = m_sendPkt.cmd + m_sendPkt.len;

	QByteArray ba;
	ba.append( static_cast< char >( START_BYTE ) );
	ba.append( static_cast< char >( m_sendPkt.cmd ) );
	ba.append( static_cast< char >( m_sendPkt.len ) );

	for(uint8_t i = 0; i < m_sendPkt.len; i++){
		m_sendPkt.data[i] = static_cast< uint8_t >( data.at(i) );
		m_sendPkt.crc += m_sendPkt.data[i];
		ba.append( static_cast< char >( m_sendPkt.data[i] ) );
	}
	ba.append( static_cast< char >( m_sendPkt.crc ) );

	ba.append( static_cast< char >( STOP_BYTE ) );

	m_pSerialPort->write(ba);
	m_pSerialPort->waitForBytesWritten(300);
	ui->logBox->append("SND >: <span style=\"color:gray\">" + printHex(ba.toHex()) +"</span><br>" );
}

void MainWindow::on_setAddrB_clicked()
{
	QByteArray ba;
	ba.append( ui->blockAddrBox->value() );
	sendPkt(CMD_SET_ADDRESS,ba);
}

void MainWindow::on_readB_clicked()
{
	sendPkt(CMD_READ_BLOCK,"");
}

void MainWindow::on_writeB_clicked()
{
	QByteArray ba;
	for(QString elem:ui->blockDataBox->text().split(" ")) ba.append( elem.toUInt(nullptr,16) );
	sendPkt(CMD_WRITE_BLOCK,ba);
}

void MainWindow::on_setUID_clicked()
{
	QByteArray ba;
	for(QString elem:ui->uidBox->text().split(" ")) ba.append( elem.toUInt(nullptr,16) );
	sendPkt(CMD_WRITE_UID,ba);
}
