// ******************************************************
// * copyright (C) 2022 by Reinhardt Behm/rbehm@hushmail.com
// * All Rights reserved
// * created 12/14/2022 by behm
// ******************************************************

#ifndef PICOFORM_H
#define PICOFORM_H

#include <QWidget>

namespace Ui {
class PicoForm;
}
class PicoPort;

class PicoForm : public QWidget
{
	Q_OBJECT

public:
	explicit PicoForm(QWidget *parent = nullptr);
	~PicoForm();
	void initBinDir(const  QString dir);
	PicoPort *port() const {  return m_port; }

protected:
	void changeEvent(QEvent *e);

private slots:
	void on_actionViewBin_triggered();
	void on_actionSelBin_triggered();
	void on_actionSelPicoDir_triggered();
	void devChanged(bool  on);
	void binDirectoryChanged(const QString &path);
	void picoDirectoryChanged(const QString &path);
	void on_reset_clicked();
	void on_actionDownload_triggered();

	void on_download_toggled(bool checked);

	void on_actiondelBin_triggered();

private:
	Ui::PicoForm *ui;
	PicoPort *m_port;
	QFileSystemWatcher *m_binDirWatcher;
	QFileSystemWatcher *m_picoDirWatcher;
	QMap<bool,QString> m_styles;
	void setBinDir(QString dn);
	void setPicoDir(QString dn);
	void chkBin();
	bool m_hasBin, m_hasPico;
	void chkDownload();

	// QWidget interface
protected:
	void dragEnterEvent(QDragEnterEvent *event) override;
	void dragMoveEvent(QDragMoveEvent *event) override;
	void dropEvent(QDropEvent *event) override;
};

#endif // PICOFORM_H
