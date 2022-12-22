// ******************************************************
// * copyright (C) 2022 by rbehm@ibb-aviotec.com
// * File: main.cpp
// * created 2022-12-14 with pro-config V0.2
// ******************************************************

#include "mainwindow.h"
#include "config.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setApplicationName("picoUi");
	app.setApplicationVersion("V0.3");
	app.setOrganizationDomain("ibb-aviotec.com");
	app.setOrganizationName("IBB-aviotech");
	QString srcDir;
	{
		const QString logo(":/logo/ibb-logo");
		app.setProperty("copyright-icon", logo);
		app.setWindowIcon(QIcon(":/logo.png"));
	}
	{
		QCommandLineParser parser;
		parser.setApplicationDescription(app.applicationName());
		parser.addHelpOption();
		parser.addVersionOption();
		parser.addPositionalArgument("srcdir", "srcdir");
		parser.process(app);
		if (! parser.positionalArguments().isEmpty())
		{
			srcDir = parser.positionalArguments().first();
		}
	}
//	{
//		QFile sf(":/styles.css");
//		sf.open(QIODevice::ReadOnly);
//		app.setStyleSheet(sf.readAll());
//	}
	Config::loadDefaults();
	MainWindow mw(srcDir);
	mw.show();
	return app.exec();
}

