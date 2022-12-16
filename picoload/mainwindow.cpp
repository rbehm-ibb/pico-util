// ******************************************************
// * copyright (C) 2022 by rbehm@ibb-aviotec.com
// * File: mainwindow.cpp
// * created 2022-11-23 with pro-config V0.2
// ******************************************************

#include <QSerialPortInfo>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "termio.h"

MainWindow::MainWindow(QString srcDir, QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, m_hasSrc(false)
	, m_hasPico(false)
{
	ui->setupUi(this);
	ui->srcDir->addAction(ui->actionSrcDir, QLineEdit::TrailingPosition);
	ui->srcDir->addAction(ui->actionOpenSrcDir, QLineEdit::TrailingPosition);
	ui->picoDir->addAction(ui->actionPicoDir, QLineEdit::TrailingPosition);
	ui->serial->addAction(ui->actionReset, QLineEdit::TrailingPosition);
	ui->serial->addAction(ui->actionOpenMinicom, QLineEdit::TrailingPosition);
	m_styles.insert(false, "* { background: #ffff60; }");
	m_styles.insert(true, "* { background: #60ff60; }");
	if (srcDir.isEmpty())
	{
		srcDir = Config::stringValue("src");
	}
	QString pico = Config::stringValue("pico", "/run/media/behm/RPI-RP2/");
	ui->srcDir->setText(srcDir);
	ui->picoDir->setText(pico);
	m_srcDir =  QDir(srcDir, "*.uf2");
	m_dstDir = QDir(ui->picoDir->text(), "RPI-RP2");
	m_dstDir.cdUp();
	startTimer(1*500);
	ui->binary->setText(QString());
//	statusBar()->hide();
}

MainWindow::~MainWindow()
{
	Config::setValue("src", ui->srcDir->text());
	Config::setValue("pico", ui->picoDir->text());
	delete ui;
}

void MainWindow::quit()
{
	close();
}


void MainWindow::on_actionSrcDir_triggered()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Build Dir", ui->srcDir->text());
	if (!  dir.isNull())
	{
		ui->srcDir->setText(dir);
		m_srcDir =  QDir(dir, "*.uf2");
		checkSrc();
	}
}

void MainWindow::on_actionPicoDir_triggered()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Pico Dir", ui->picoDir->text());
	if (!  dir.isNull())
	{
		ui->picoDir->setText(dir);
		m_dstDir = QDir(ui->picoDir->text(), "RPI-RP2");
		m_dstDir.cdUp();
	}
}


void MainWindow::timerEvent(QTimerEvent *event)
{
	Q_UNUSED(event);
	checkSrc();
	checkPico();
	checkSerial();
	if (m_hasSrc && m_hasPico)
	{
		QFile f(m_srcDir.absoluteFilePath(ui->binary->text()));
		if (! f.open(QIODevice::ReadOnly))
		{
			qWarning() << Q_FUNC_INFO << f.fileName() << f.errorString();
			return;
		}
		QDir ddir(m_dstDir);
		ddir.cd("RPI-RP2");
		QString df(ddir.absoluteFilePath(ui->binary->text()));
		qApp->processEvents();
		if (! f.copy(df))
		{
			qWarning() << Q_FUNC_INFO << f.fileName() << df << "*** no copy";
			return;
		}
		f.remove();
	}
}

void MainWindow::checkSrc()
{
	m_srcDir.refresh();
	const QStringList uf2 = m_srcDir.entryList();
	if (uf2.isEmpty())
	{
		ui->binary->setText(QString());
		m_hasSrc = false;
	}
	else
	{
		ui->binary->setText(uf2.first());
		m_hasSrc = true;
	}
	ui->binLabel->setStyleSheet(m_styles.value(m_hasSrc));
}

void MainWindow::checkPico()
{
	m_dstDir.refresh();
	const QStringList fl = m_dstDir.entryList();
	if (! fl.isEmpty())
	{
		m_hasPico = true;
	}
	else
	{
		m_hasPico = false;
	}
	ui->picoLabel->setStyleSheet(m_styles.value(m_hasPico));
}

void MainWindow::checkSerial()
{
	m_hasSerial = false;
	foreach (const QSerialPortInfo &spi, QSerialPortInfo::availablePorts())
	{
		if (spi.hasVendorIdentifier() && spi.vendorIdentifier() == 0x2e8a)
		{
//			qDebug() << Q_FUNC_INFO << spi.portName() << spi.serialNumber() << spi.manufacturer() << Qt::hex << spi.vendorIdentifier() << spi.productIdentifier() << Qt::dec;
			m_hasSerial = true;
			ui->serial->setText(spi.systemLocation() + "\tS/N:" + spi.serialNumber());
			ui->serial->setEnabled(true);
			break;
		}
	}
	ui->serLabel->setStyleSheet(m_styles.value(m_hasSerial));
	ui->actionOpenMinicom->setEnabled(m_hasSerial);
	if (! m_hasSerial)
	{
		ui->serial->clear();
		ui->serial->setDisabled(true);
//		ui->actionOpenMinicom->setDisabled(true);
	}
}

void MainWindow::on_actionOpenSrcDir_triggered()
{
	QDesktopServices::openUrl(m_srcDir.absolutePath());
}


void MainWindow::on_actionOpenMinicom_triggered()
{
	static const QChar sep('\t');
//	qDebug() << Q_FUNC_INFO;
	QString cmd("konsole -e $SHELL -c \"/usr/bin/minicom -b 115200 -D %1 -w\" &");
	QString dev = ui->serial->text().section(sep, 0, 0);
	cmd = cmd.arg(dev);
//	qDebug() << Q_FUNC_INFO << cmd;
	system(cmd.toLocal8Bit().constData());
}


void MainWindow::on_actionReset_triggered()
{
	static const QChar sep('\t');
	QString dev = ui->serial->text().section(sep, 0, 0);
//	qDebug() << Q_FUNC_INFO << dev;
	int fd = ::open(dev.toLocal8Bit().constData(), O_RDWR);
	if (fd < 0)
	{
//		statusBar()->show();
		statusBar()->showMessage(dev + ": no open", 20*1000);
		qWarning() << Q_FUNC_INFO << "no open";
		return;
	}
	struct termios tc;
	int rc = ::tcgetattr(fd, &tc);
	if (rc < 0)
	{
		statusBar()->showMessage(dev + ": no tcgetattr", 20*1000);
		qWarning() << Q_FUNC_INFO << "no tcgetattr";
		::close(fd);
		return;
	}
	rc = ::cfsetspeed(&tc, B1200);
	if (rc < 0)
	{
		statusBar()->showMessage(dev + ": no cfsetspeed", 20*1000);
		qWarning() << Q_FUNC_INFO << "no cfsetspeed";
		::close(fd);
		return;
	}
	rc =  ::tcsetattr(fd, TCSAFLUSH, &tc);
	if (rc < 0)
	{
		statusBar()->showMessage(dev + ": no tcsetattr", 20*1000);
		qWarning() << Q_FUNC_INFO << "no tcsetattr";
		::close(fd);
		return;
	}
	::close(fd);
}
