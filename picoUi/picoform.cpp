// ******************************************************
// * copyright (C) 2022 by Reinhardt Behm/rbehm@hushmail.com
// * All Rights reserved
// * created 12/14/2022 by behm
// ******************************************************

#include "picoform.h"
#include "ui_picoform.h"
#include "config.h"
#include "picoport.h"

PicoForm::PicoForm(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PicoForm)
	, m_port(new PicoPort(this))
	, m_binDirWatcher(new QFileSystemWatcher(this))
	, m_picoDirWatcher(new QFileSystemWatcher(this))
	, m_hasBin(false)
	, m_hasPico(false)
{
	ui->setupUi(this);
	m_styles.insert(false, "* { background: #ffffc0; }");
	m_styles.insert(true, "* { background: #a0ff80; }");
	ui->binDir->addAction(ui->actionSelBin, QLineEdit::TrailingPosition);
	ui->binDir->addAction(ui->actionViewBin, QLineEdit::TrailingPosition);
	ui->binFile->addAction(ui->actionDownload, QLineEdit::TrailingPosition);
	ui->binFile->addAction(ui->actiondelBin, QLineEdit::TrailingPosition);
	ui->picoDir->addAction(ui->actionSelPicoDir, QLineEdit::TrailingPosition);
	connect(m_port, &PicoPort::devChanged, this, &PicoForm::devChanged);
	connect(m_binDirWatcher, &QFileSystemWatcher::directoryChanged, this, &PicoForm::binDirectoryChanged);
	connect(m_picoDirWatcher, &QFileSystemWatcher::directoryChanged, this, &PicoForm::picoDirectoryChanged);
	devChanged(m_port->isOpen());
	setBinDir(Config::stringValue("picoForm/bindir"));
	setPicoDir(Config::stringValue("picoForm/picoDir"));
	setAcceptDrops(true);
	ui->picoPort->setStyleSheet("* { background: white; }");
	chkDownload();
	ui->download->setChecked(Config::boolValue("picoForm/auto"));
	ui->autoDl->setChecked(Config::boolValue("picoForm/autodl"));
	QFileSystemWatcher *devW = new QFileSystemWatcher(this);
	connect(devW, &QFileSystemWatcher::directoryChanged, this, &PicoForm::devDirectoryChanged);
	devW->addPath("/dev");
}

PicoForm::~PicoForm()
{
	Config::setValue("picoForm/bindir", ui->binDir->text());
	Config::setValue("picoForm/picoDir", ui->picoDir->text());
	Config::setValue("picoForm/auto", ui->download->isChecked());
	Config::setValue("picoForm/autodl", ui->autoDl->isChecked());
	delete ui;
}

void PicoForm::initBinDir(const QString dir)
{
	setBinDir(dir);
}

void PicoForm::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void PicoForm::on_actionViewBin_triggered()
{
	QDesktopServices::openUrl(ui->binDir->text());
}

void PicoForm::on_actionSelBin_triggered()
{
	QString dn = QFileDialog::getExistingDirectory(this, "Bin directory", ui->binDir->text());
	if (!dn.isNull())
	{
		setBinDir(dn);
	}
}

void PicoForm::on_actionSelPicoDir_triggered()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Pico Dir", ui->picoDir->text());
	if (! dir.isNull())
	{
		setPicoDir(dir);
	}
}

void PicoForm::devChanged(bool on)
{
	ui->picolab->setStyleSheet(m_styles.value(on));
	ui->picoPort->setText(m_port->device());
	ui->reset->setEnabled(on);
}

void PicoForm::binDirectoryChanged(const QString &path)
{
	Q_UNUSED(path)
//	qDebug() << Q_FUNC_INFO << path;
	chkBin();
}

void PicoForm::picoDirectoryChanged(const QString &path)
{
	Q_UNUSED(path)
//	qDebug() << Q_FUNC_INFO << path;
	setPicoDir(ui->picoDir->text());
	QDir dir(ui->picoDir->text());
	m_hasPico = dir.exists();
	ui->picodirlab->setStyleSheet(m_styles.value(m_hasPico));
	if (ui->autoDl->isChecked() && m_hasBin && m_hasPico)
	{
		on_actionDownload_triggered();
	}
	else
	{
		chkDownload();
	}
}

void PicoForm::setBinDir(QString dn)
{
	ui->binDir->setText(dn);
//	qDebug() << Q_FUNC_INFO << m_watcher->files() << m_watcher->directories();
	if (!m_binDirWatcher->directories().isEmpty())
	{
		m_binDirWatcher->removePaths(m_binDirWatcher->directories());
	}
	m_binDirWatcher->addPath(dn);
//	qDebug() << Q_FUNC_INFO << m_watcher->files() << m_watcher->directories();
	chkBin();
}

void PicoForm::setPicoDir(QString dn)
{
	ui->picoDir->setText(dn);
	int i = dn.lastIndexOf('/');
	if (i >= 0)
	{
		dn = dn.left(i);
	}
	QDir dir(dn);
	// cdUp() does not work is dir not exists !
	while (! dir.exists())
	{
		int i = dn.lastIndexOf('/');
		if (i >= 0)
		{
			dn = dn.left(i);
		}
		dir.setPath(dn);
	}
	const QStringList sdir = m_picoDirWatcher->directories();
	if (! sdir.isEmpty())
	{
		m_picoDirWatcher->removePaths(sdir);
	}
	m_picoDirWatcher->addPath(dir.absolutePath());
//	qDebug() << Q_FUNC_INFO << dir << dn << m_picoDirWatcher->directories();
}

void PicoForm::chkBin()
{
	const QStringList sdir = m_binDirWatcher->directories();
	m_hasBin =  false;
	if (! sdir.isEmpty())
	{
		QDir dir (sdir.first(), "*.uf2", QDir::NoSort, QDir::Files);
		const QStringList fns = dir.entryList();
		if (! fns.isEmpty())
		{
			ui->binFile->setText(fns.first());
			ui->binflab->setStyleSheet(m_styles.value(true));
			if (! m_hasBin)	// newly on?
			{
				m_hasBin = true;
				if (ui->download->isChecked() && ! m_hasPico)
				{
					chkDownload();
					on_reset_clicked();
				}
			}
			m_hasBin = true;
		}
		else
		{
			ui->binFile->setText(QString());
			ui->binflab->setStyleSheet(m_styles.value(false));
		}
	}
	chkDownload();
	return;
}

void PicoForm::chkDownload()
{
//	qDebug() << Q_FUNC_INFO << m_hasBin << m_hasPico << ui->download->isChecked();
	ui->actionDownload->setEnabled(m_hasBin && m_hasPico);
	ui->actiondelBin->setEnabled(m_hasBin);
	if (m_hasBin && m_hasPico && ui->download->isChecked())
	{
		on_actionDownload_triggered();
	}
}

void PicoForm::dragEnterEvent(QDragEnterEvent *event)
{
	event->setAccepted(event->mimeData()->hasUrls());
}

void PicoForm::dragMoveEvent(QDragMoveEvent *event)
{
	if ((childAt(event->pos()) == ui->binDir) && event->mimeData()->hasUrls())
	{
		const QList<QUrl> urls = event->mimeData()->urls();
		if (urls.count() == 1)
		{
			const QUrl url = urls.first();
			if (url.isLocalFile())
			{
//				qDebug() << Q_FUNC_INFO << url.toLocalFile();
				QFileInfo fi( url.toLocalFile());
				if (fi.isDir())
				{
					event->accept();
					return;
				}
			}
		}
	}
	event->ignore();
}

void PicoForm::dropEvent(QDropEvent *event)
{
	if ((childAt(event->pos()) == ui->binDir) && event->mimeData()->hasUrls())
	{
		const QList<QUrl> urls = event->mimeData()->urls();
		if (urls.count() == 1)
		{
			const QUrl url = urls.first();
			if (url.isLocalFile())
			{
				QFileInfo fi( url.toLocalFile());
				if (fi.isDir())
				{
					setBinDir(fi.absoluteFilePath());
				}
			}
		}
	}
}

void PicoForm::on_reset_clicked()
{
	m_port->boot();
}

void PicoForm::on_actionDownload_triggered()
{
	if (m_hasBin && m_hasPico)
	{
//		qDebug() << Q_FUNC_INFO;
		QDir dir(ui->binDir->text());
		QString bfn(dir.absoluteFilePath(ui->binFile->text()));
		QFile f(bfn);
		if (! f.exists())
		{
			qWarning() << Q_FUNC_INFO << f.fileName() << "not found";
			return;
		}
		dir.setPath(ui->picoDir->text());
		QString dfn(dir.absoluteFilePath(ui->binFile->text()));
//		qDebug() << Q_FUNC_INFO << f.fileName() << dfn;
		sleep(1);
		if (! f.copy(dfn))
		{
			qWarning() << Q_FUNC_INFO << f.fileName() << dfn << "*** no copy";
			return;
		}
		f.remove();
	}
}

void PicoForm::on_download_toggled(bool checked)
{
	if (checked)
	{
		ui->autoDl->setChecked(true);
	}
}

void PicoForm::on_actiondelBin_triggered()
{
	qDebug() << Q_FUNC_INFO;
	QDir dir(ui->binDir->text());
	QString bfn(dir.absoluteFilePath(ui->binFile->text()));
	QFile f(bfn);
	if (! f.exists())
	{
		qWarning() << Q_FUNC_INFO << f.fileName() << "not found";
		return;
	}
	f.remove();
}

void PicoForm::devDirectoryChanged(const QString &path)
{
	qDebug() << Q_FUNC_INFO << path;
}
