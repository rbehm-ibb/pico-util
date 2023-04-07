// ******************************************************
// * copyright (C) 2017 by Reinhardt Behm/rbehm@hushmail.com
// * All Rights reserved
// * created 1/25/2017 by behm
// ******************************************************

#include "console.h"
#include "config.h"

Console::Console(QWidget *parent)
	: QPlainTextEdit(parent)
	, maxcol(80)
	, maxrow(1000)
	, tabsize(8)
	, isEcho(true)
{
	setWindowTitle(QCoreApplication::applicationName() + ": Terminal");
//	setWindowIcon(QIcon(":/icons/pics/project-terminal.png"));
//	QRect r = Config::value("ui_pos/console", QRect(QPoint(0, 0), QSize(640, 480))).toRect();
//	move(r.topLeft());
//	resize(r.size()); // initial size

	setWordWrapMode(QTextOption::WordWrap);

	QAction *clear = new QAction(QIcon(":/remove"), "Clear", this);
	clear->setShortcut(QKeySequence("Alt+C"));
	connect(clear, &QAction::triggered, this, &Console::clear);
	addAction(clear);
}

void Console::clear()
{
	setPlainText("");
}

void Console::keyPressEvent(QKeyEvent *event)
{
//	qDebug() << Q_FUNC_INFO << event;

	if(event->matches((QKeySequence::Paste)))
	{
		QClipboard *clip = QApplication::clipboard();
		QString s = clip->text();
		s = s.replace("\n", "\r");
		sendPort(s);
	}
	else
	{
		int key = event->key();
//		qDebug() << Q_FUNC_INFO << 1 << key << event->text();

		switch(key)
		{
		case Qt::Key_Enter:
		case Qt::Key_Return:
			key = '\r';
			break;
		case Qt::Key_Alt:
		case Qt::Key_Control:
		case Qt::Key_Shift:
			return;
		default:
			if(QApplication::keyboardModifiers() & Qt::CTRL)
			{
				key &= ~0xe0;
			}
			else
			{
				QString s = event->text();
				if(s.length() == 1)
				{
					QChar c = event->text().at(0);
					key = (int)c.toLatin1();
					if (isEcho)
					{
						charRxd(key);
					}
//					qDebug() << Q_FUNC_INFO << 2 << key << c << event->text();
				}
				else
				{
					sendPort(s);
					return;
				}
			}
			break;
		}
		QByteArray ba;
		ba.append((char)key);
//		qDebug() << Q_FUNC_INFO << 3 << key << barry.toHex();
		emit sendSerial(ba);
	}
}

void Console::sendPort(QString s)
{
	QByteArray ba;
	foreach(QChar c, s.toUtf8())
	{
		ba.append(c.toLatin1());
		emit sendSerial(ba);
//		ba.clear();
//		thread()->yieldCurrentThread();
	}
}

void Console::resizeEvent(QResizeEvent *e)
{
	QFontMetrics fm(font());
	maxcol = width() / 8;
	if(fm.horizontalAdvance("X") > 0)
	{
		maxcol = width() / fm.horizontalAdvance("X") - 3;
	}

	//qDebug() << maxcol << width() << fm.width("X");
	QPlainTextEdit::resizeEvent(e);
}

void Console::charRxd(char ch)
{
//	qDebug() << Q_FUNC_INFO << hex << uint(ch) << dec;

	switch (ch)
	{
	case 0x0d:
		return;		// ignore
	case 0x0c:
		setPlainText("");
		return;
	}
	QString text = "";
	moveCursor(QTextCursor::End);
	QTextCursor cur = textCursor();

	setMaximumBlockCount(maxrow);
	setWordWrapMode(QTextOption::WrapAnywhere);

	if(wrapMode > 0)
	{
		if(cur.block().length() > wrapMode)
			cur.insertBlock();
	}
//	else if(cur.block().length() > maxcol)
//	{
//		cur.insertBlock();
//	}
	if(cur.block().length() - 1 > cur.columnNumber())
	{
		cur.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
	}
	cur.insertText(QString(ch));
	if (ch == '?')
		emit beep();
	setTextCursor(cur);
	return;
}
