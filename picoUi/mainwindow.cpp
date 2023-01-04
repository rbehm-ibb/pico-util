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
	ui->txline->setText(Config::stringValue("txline"));
	ui->sendline->clear();
	ui->sendline->addItems(Config::value("txlines").toStringList());
	connect(ui->picoForm->port(), &QSerialPort::readyRead, this, &MainWindow::readRxdDataSlot);
//	connect(ui->picoForm->port(), &PicoPort::devChanged, this, &MainWindow::devChanged);
	connect(ui->console, &Console::sendSerial, ui->picoForm->port(), &PicoPort::sendSerial);
	connect(ui->sendButton, &QPushButton::clicked, this, &MainWindow::on_txline_returnPressed);
}

MainWindow::~MainWindow()
{
	Config::setValue("txline", ui->txline->text());
	QStringList items;
	QComboBox *box = ui->sendline;
	const int nrow = box->model()->rowCount();
	for (int r = 0; r < nrow; ++r)
	{
		items.append(box->itemText(r));
	}
	items.sort(Qt::CaseInsensitive);
	Config::setValue("txlines", items);
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


void MainWindow::on_txline_returnPressed()
{
	QByteArray line = ui->txline->text().toLocal8Bit();
	ui->picoForm->port()->sendSerial(line + '\r');
	QComboBox *box = ui->sendline;
	const int nrow = box->model()->rowCount();
	bool insert = true;
	for (int r = 0; r < nrow; ++r)
	{
		if (box->itemText(r) == line)
		{
			insert = false;
			break;
		}
	}
	if (insert)
	{
		ui->sendline->insertItem(-1, line);
	}
}

void MainWindow::on_sendline_activated(int index)
{
	qDebug() << Q_FUNC_INFO << index;
	ui->txline->setText(ui->sendline->currentText());
}


void MainWindow::on_actionClearHistory_triggered()
{
	ui->sendline->clear();
}

