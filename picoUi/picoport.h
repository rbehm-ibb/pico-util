// ******************************************************
// * copyright (C) 2022 by Reinhardt Behm/rbehm@hushmail.com
// * All Rights reserved
// * created 12/14/2022 by behm
// ******************************************************

#ifndef PICOPORT_H
#define PICOPORT_H

class PicoPort : public QSerialPort
{
	Q_OBJECT
public:
	PicoPort(QObject *parent);
	~PicoPort();
	QString device() const;
	void boot();
	void chkPort() { timerEvent(nullptr); }
signals:
	void  devChanged(bool  on);
public slots:
	void sendSerial(QByteArray bytes);
private slots:
//	void lostPortErrorSl();
protected:
	void timerEvent(QTimerEvent *event) override;
	static const uint16_t m_vid = 0x2e8a;
	static const uint16_t m_pid = 0x000a;
	QSerialPortInfo m_devInfo;
	qint32 m_baud;
};

#endif // PICOPORT_H
