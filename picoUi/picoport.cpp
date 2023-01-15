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
//	startTimer(200);
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
	}
}

void PicoPort::open(QSerialPortInfo si)
{
	if (isOpen() && portName() != si.portName())
	{
		qDebug() << Q_FUNC_INFO << portName() << si.portName();
		close();
	}
	qDebug() << Q_FUNC_INFO << si.portName();
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

#if 0
void PicoPort::open(QString port)
{
	if (isOpen() && portName() != port)
	{
		qDebug() << Q_FUNC_INFO << portName() << port;
		close();
	}
	if (! isOpen())
	{
		QSerialPort::setPortName(port);
		setBaudRate(m_baud);
		if (! QSerialPort::open(QIODevice::ReadWrite))
		{
			qWarning() << Q_FUNC_INFO << portName() << errorString();
		}
		else
		{
//			m_devInfo = QSerialPort::
			setPort(m_devInfo);
			setBaudRate(m_baud);
			setParity(QSerialPort::NoParity);
			setDataBits(QSerialPort::Data8);
			setFlowControl(QSerialPort::NoFlowControl);
		}
	}
	emit devChanged(isOpen());
}
#endif

void PicoPort::setSerial(const QString sn)
{
	m_sn = sn;
}

void PicoPort::sendSerial(QByteArray bytes)
{
	QSerialPort::write(bytes);
}

#if 0
void PicoPort::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event)
	static bool inside = false;
	if (inside)
	{
		qDebug() << Q_FUNC_INFO << inside;
		return;
	}
	inside = true;
	if (m_devInfo.isNull())	// non found yet
	{
		foreach (QSerialPortInfo spi, QSerialPortInfo::availablePorts())
		{
	//		qDebug() << Q_FUNC_INFO << hex << spi.portName() << spi.vendorIdentifier() << spi.productIdentifier() << spi.serialNumber();
			if (spi.vendorIdentifier() == m_vid && spi.productIdentifier() == m_pid)
			{
				m_devInfo = spi;
//				setObjectName(spi.description());
//				setPort(spi);
//				if (! open(QIODevice::ReadWrite))
//				{
//					qWarning() << Q_FUNC_INFO << portName() << errorString();
//					return;
//				}
				break;
			}
		}
	}
	bool exist = QFile::exists(m_devInfo.systemLocation());
	if (isOpen())
	{
//		qDebug() << Q_FUNC_INFO << "o" << exist << m_devInfo.systemLocation();
		if (! exist)
		{
			close();
			emit devChanged(false);
		}
	}
	else if (exist)
	{
//		qDebug() << Q_FUNC_INFO << "c" << exist;
		QElapsedTimer timer;
		timer.start();
		while (timer.elapsed() < 1000)
		{
		}
		setPort(m_devInfo);
		setBaudRate(m_baud);
		while (! open(QIODevice::ReadWrite))
		{
//			qWarning() << Q_FUNC_INFO << timer.elapsed() * 1e-3 << portName() << errorString();
			if (timer.elapsed() > 10 * 1000)
				break;;
		}
		setBaudRate(m_baud);
		setParity(QSerialPort::NoParity);
		setDataBits(QSerialPort::Data8);
		setFlowControl(QSerialPort::NoFlowControl);
		emit devChanged(isOpen());
	}
	inside = false;
}
#endif

