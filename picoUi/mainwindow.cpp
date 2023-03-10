// ******************************************************
// * copyright (C) 2022 by rbehm@ibb-aviotec.com
// * File: mainwindow.cpp
// * created 2022-12-14 with pro-config V0.2
// ******************************************************

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "picoport.h"

MainWindow::MainWindow(QString binDir, QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	if (! binDir.isEmpty())
	{
		ui->picoForm->initBinDir(binDir);
	}
	connect(ui->picoForm->port(), &QSerialPort::readyRead, this, &MainWindow::readRxdDataSlot);
//	connect(ui->picoForm->port(), &PicoPort::devChanged, this, &MainWindow::devChanged);
	connect(ui->console, &Console::sendSerial, ui->picoForm->port(), &PicoPort::sendSerial);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::quit()
{
	close();
}

void MainWindow::readRxdDataSlot()
{
	QByteArray rxd = ui->picoForm->port()->readAll();
	foreach (const uint8_t &c, rxd)
	{
		ui->console->charRxd(c);
	}
}

void MainWindow::on_actionClear_triggered()
{
	ui->console->clear();
}

void MainWindow::on_actionSizeH_triggered()
{
	QDesktopWidget *dt = qApp->desktop();
	QSize sz = dt->screenGeometry(this).size();
//	qDebug() << Q_FUNC_INFO << dt->screenNumber(this) << dt->screenGeometry(this) << dt->availableGeometry(this);
	QSize msz = size();
	msz.setHeight(sz.height());
	resize(msz);
}

