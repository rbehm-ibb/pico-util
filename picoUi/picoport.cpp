// ******************************************************
// * copyright (C) 2022 by Reinhardt Behm/rbehm@hushmail.com
// * All Rights reserved
// * created 12/14/2022 by behm
// ******************************************************

#include "picoport.h"

PicoPort::PicoPort(QObject *parent)
	: QSerialPort(parent)
{
	m_baud = 115200;
//	qDebug() << Q_FUNC_INFO << device() << m_baud;
}

PicoPort::~PicoPort()
{
//-	qDebug() << Q_FUNC_INFO;
}

QString PicoPort::device() const
{
	QString s("%1:%2\tSN:%3");
	if (m_devInfo.isNull())
	{
		return "--:--";
	}
	return s.arg(m_devInfo.portName()).arg(m_baud).arg(m_devInfo.serialNumber());
}

void PicoPort::boot()
{
	if (isOpen())
	{
		QSerialPort::setBaudRate(1200);
//		while  (QFile::exists(m_devInfo.systemLocation()))
//			;
		close();
	}
}

void PicoPort::open(QSerialPortInfo si)
{
	if (isOpen() && portName() != si.portName())
	{
		qDebug() << Q_FUNC_INFO << portName() << si.portName();
		close();
	}
	qDebug() << Q_FUNC_INFO << si.portName() << isOpen();
	if (! isOpen())
	{
		setPort(si);
		m_devInfo = si;
		setBaudRate(m_baud);
		if (! QSerialPort::open(QIODevice::ReadWrite))
		{
			qWarning() << Q_FUNC_INFO << portName() << errorString();
		}
		else
		{
			setPort(m_devInfo);
			setBaudRate(m_baud);
			setParity(QSerialPort::NoParity);
			setDataBits(QSerialPort::Data8);
			setFlowControl(QSerialPort::NoFlowControl);
		}
	}
	emit devChanged(isOpen());
}

void PicoPort::sendSerial(QByteArray bytes)
{
	QSerialPort::write(bytes);
}
