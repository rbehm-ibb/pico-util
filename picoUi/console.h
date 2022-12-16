// ******************************************************
// * copyright (C) 2017 by Reinhardt Behm/rbehm@hushmail.com
// * All Rights reserved
// * created 1/25/2017 by behm
// ******************************************************

#ifndef CONSOLE
#define CONSOLE

class Console : public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit Console(QWidget *parent = 0);

signals:
	void sendSerial(QByteArray bytes);
	void beep();

public slots:
	void clear();
	void charRxd(char ch);
private:

	void sendPort(QString s);
	int  maxcol;
	int  maxrow;
	int  wrapMode;
	int  tabsize;
	bool isEcho;

protected:
	void keyPressEvent(QKeyEvent *event);
	void resizeEvent(QResizeEvent *e);
};
#endif
