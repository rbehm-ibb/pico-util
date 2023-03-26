// ******************************************************
// * copyright (C) 2022 by Reinhardt Behm/rbehm@hushmail.com
// * All Rights reserved
// * created 12/14/2022 by behm
// ******************************************************

#include "picoform.h"
#include "ui_picoform.h"
#include "config.h"
#include "picoport.h"
#include "filenamehandler.h"

PicoForm::PicoForm(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PicoForm)
	, m_port(new PicoPort(this))
	, m_binDirWatcher(new QFileSystemWatcher(this))
	, m_picoDirWatcher(new QFileSystemWatcher(this))
	, m_hasBin(false)
	, m_hasPico(false)
	, m_inside(false)
	, m_booting(false)
{
	ui->setupUi(this);
	m_styles.insert(false, "* { background: #ffffc0; }");
	m_styles.insert(true, "* { background: #a0ff80; }");
	ui->binDir->addAction(ui->actionSelBin, QLineEdit::TrailingPosition);
	ui->binDir->addAction(ui->actionViewBin, QLineEdit::TrailingPosition);
	ui->binFile->addAction(ui->actionDownload, QLineEdit::TrailingPosition);
	ui->binFile->addAction(ui->actiondelBin, QLineEdit::TrailingPosition);
//	ui->binFile->addAction(ui->actionNoDel, QLineEdit::TrailingPosition);
	ui->picoDir->addAction(ui->actionSelPicoDir, QLineEdit::TrailingPosition);
	ui->picoPort->addAction(ui->actionBoot, QLineEdit::TrailingPosition);
	connect(m_port, &PicoPort::devChanged, this, &PicoForm::devChanged);
	connect(m_binDirWatcher, &QFileSystemWatcher::directoryChanged, this, &PicoForm::binDirectoryChanged);
	connect(m_picoDirWatcher, &QFileSystemWatcher::directoryChanged, this, &PicoForm::picoDirectoryChanged);
	devChanged(m_port->isOpen());
	setBinDir(Config::stringValue("picoForm/bindir"));
	setPicoDir(Config::stringValue("picoForm/picoDir"));
	setAcceptDrops(true);
	ui->picoPort->setStyleSheet("* { background: white; }");
	chkDownload();
//	ui->download->setChecked(Config::boolValue("picoForm/auto"));
	ui->autoDl->setChecked(Config::boolValue("picoForm/autodl"));
	QFileSystemWatcher *devW = new QFileSystemWatcher(this);
	connect(devW, &QFileSystemWatcher::directoryChanged, this, &PicoForm::devDirectoryChanged);
	devW->addPath("/dev");
	m_sn = Config::stringValue("pico/serial");
	ui->portSel->setCurrentText(m_sn);
	devDirectoryChanged(QString());
//	ui->portSel->setCurrentText(Config::stringValue("picoForm/port"));
//	ui->actionNoDel->setChecked(Config::boolValue("picoForm/noDelBin"));
	addAction(ui->actionBoot);
	connect(ui->actionBoot, &QAction::triggered, this, &PicoForm::actionBoot_triggered);

}

PicoForm::~PicoForm()
{
	Config::setValue("picoForm/bindir", ui->binDir->text());
	Config::setValue("picoForm/picoDir", ui->picoDir->text());
//	Config::setValue("picoForm/noDelBin", ui->actionNoDel->isChecked());
	Config::setValue("picoForm/autodl", ui->autoDl->isChecked());
	Config::setValue("picoForm/port", ui->portSel->currentText());
	Config::setValue("pico/serial", m_sn);
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
	ui->actionBoot->setEnabled(on);
//	qDebug() << Q_FUNC_INFO << m_port->device() << m_port->isOpen() << on << ui->actionBoot->isEnabled() << ui->picoPort->isEnabled();
}

void PicoForm::binDirectoryChanged(const QString &path)
{
	Q_UNUSED(path)
	qDebug() << Q_FUNC_INFO << path;
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
				if (ui->autoDl->isChecked() && ! m_hasPico)
				{
					chkDownload();
					m_port->boot();
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
	ui->actionDownload->setEnabled(m_hasBin /*&& m_hasPico*/);
	ui->actiondelBin->setEnabled(m_hasBin);
	if (m_hasBin && m_hasPico && ui->autoDl->isChecked())
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

//void PicoForm::on_reset_clicked()
//{
//	m_port->boot();
//}

void PicoForm::on_actionDownload_triggered()
{
	qDebug() << Q_FUNC_INFO;
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
		FilenameHandler fnh(f.fileName(), ".elf");
		if (! f.copy(dfn))
		{
			qWarning() << Q_FUNC_INFO << f.fileName() << dfn << "*** no copy";
			return;
		}
		f.remove();
		f.remove(fnh.fullname());
		m_booting = false;
	}
}

//void PicoForm::on_download_toggled(bool checked)
//{
//	qDebug() << Q_FUNC_INFO << checked;
//	if (checked)
//	{
//		ui->autoDl->setChecked(true);
//	}
//}

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

//Q_DECLARE_METATYPE(QSerialPortInfo);

void PicoForm::devDirectoryChanged(const QString &path)
{
	qDebug() << Q_FUNC_INFO << m_inside << m_booting;
	if (m_inside)
	{
		return;
	}
	m_inside = true;
	Q_UNUSED(path);
	const uint16_t vidPi = 0x2e8a;
	const uint16_t vidSeed = 0x2886;
	const uint16_t vidEAE = 0x0ae6;
	const QVector<uint16_t> vid({ vidPi, vidSeed, vidEAE });
	qDebug() << Q_FUNC_INFO;
	QString saved = ui->portSel->currentText();
	ui->portSel->clear();
	QSerialPortInfo csi;
	bool portFound = false;
	foreach (const QSerialPortInfo &spi, QSerialPortInfo::availablePorts())
	{
		if (m_port->devInfo().portName() == spi.portName())
		{
			portFound = true;
		}
		if (vid.contains(spi.vendorIdentifier()))
		{
			QString s("%1 %2:%3 #%4");
			s = s.arg(spi.portName()).arg(spi.vendorIdentifier(), 4, 16, QChar('0')).arg(spi.productIdentifier(), 4, 16, QChar('0')).arg(spi.serialNumber());
			qDebug() << Q_FUNC_INFO << s << m_sn;
			ui->portSel->addItem(s);
			if (spi.serialNumber() == m_sn)
			{
				csi = spi;
			}
		}
	}
	if (! portFound)
	{
		m_port->close();
		m_booting = false;
	}
	ui->portSel->setCurrentText(saved);
	if (m_port && m_port->isOpen())
	{
		QSerialPortInfo si = m_port->devInfo();
		if (si.isNull() && si.serialNumber() != m_sn)
		{
			m_port->open(csi);
		}
	}
	else
	{
		m_port->open(csi);
	}
	devChanged(m_port->isOpen());
	m_inside = false;
}

void PicoForm::on_portSel_activated(int index)
{
//	qDebug() << Q_FUNC_INFO << index << m_sn;
	if (m_inside)
		return;
	if (index >= 0)
	{
		QString sn = ui->portSel->currentText().section('#', -1);
		if (sn != m_sn)
		{
			m_sn = sn;
//			qDebug() << Q_FUNC_INFO << index << m_sn;
			foreach (const QSerialPortInfo &spi, QSerialPortInfo::availablePorts())
			{
				if (spi.serialNumber() == m_sn)
				{
					m_port->open(spi);
					return;
				}
			}
		}
	}
}

void PicoForm::actionBoot_triggered()
{
	qDebug() << Q_FUNC_INFO;
	m_port->boot();
}
