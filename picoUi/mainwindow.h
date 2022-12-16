// ******************************************************
// * copyright (C) 2022 by rbehm@ibb-aviotec.com
// * File: mainwindow.h
// * created 2022-12-14 with pro-config V0.2
// ******************************************************

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

public slots:
	void quit();
private slots:
	void readRxdDataSlot();
//	void  devChanged(bool on);
//	void sendSerial(QByteArray bytes);

	void on_actionClear_triggered();

private:
	Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

