// ******************************************************
// * copyright (C) 2022 by rbehm@ibb-aviotec.com
// * File: mainwindow.h
// * created 2022-11-23 with pro-config V0.2
// ******************************************************

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QString srcDir, QWidget *parent = nullptr);
	~MainWindow();

public slots:
	void quit();
private slots:
	void on_actionSrcDir_triggered();

	void on_actionPicoDir_triggered();

	void on_actionOpenSrcDir_triggered();

	void on_actionOpenMinicom_triggered();

	void on_actionReset_triggered();

private:
	Ui::MainWindow *ui;
	QDir m_srcDir;
	QDir m_dstDir;

	// QObject interface
protected:
	void timerEvent(QTimerEvent *event) override;
	void checkSrc();
	void checkPico();
	void checkSerial();
	QMap<bool,QString> m_styles;
	bool m_hasSrc;
	bool m_hasPico;
	bool m_hasSerial;
};

#endif // MAINWINDOW_H

