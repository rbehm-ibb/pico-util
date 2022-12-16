# // ******************************************************
# // * copyright (C) 2022 by Reinhardt Behm/rbehm@hushmail.com
# // * All Rights reserved
# // ******************************************************

TEMPLATE = app

# include( $$[PRTEMPLATE]/libusb.pri)
include( $$[PRTEMPLATE]/project.pri)
# include( $$[PRTEMPLATE]/lib/csvmodel/csvmodel.pri)
# include( $$[PRTEMPLATE]/lib/instruments/meter/meter.pri)
# include( $$[PRTEMPLATE]/lib/instruments/scalebar/scalebar.pri)
# include( $$[PRTEMPLATE]/lib/instruments/scalegauge/scalegauge.pri)
# include( $$[PRTEMPLATE]/lib/osmview/osmwidget.pri)
# include( $$[PRTEMPLATE]/lib/QHexView/qhexview.pri)
# include( $$[PRTEMPLATE]/lib/serialport/ibserialport.pri)
# include( $$[PRTEMPLATE]/lib/serialport/ibserialportline.pri)
# include( $$[PRTEMPLATE]/lib/serialport/ibserialportnmea.pri)
# include( $$[PRTEMPLATE]/lib/serialport/ipccomm.pri)
# include( $$[PRTEMPLATE]/lib/serialport/serialportselector.pri)
# include( $$[PRTEMPLATE]/lib/smtp/smtp.pri)
# include( $$[PRTEMPLATE]/lib/streams/telnetclient.pri)
# include( $$[PRTEMPLATE]/lib/streams/telnetserver.pri)
# include( $$[PRTEMPLATE]/lib/utils/betl-logo.pri)
include( $$[PRTEMPLATE]/lib/utils/config.pri)
# include( $$[PRTEMPLATE]/lib/utils/config_w.pri)
# include( $$[PRTEMPLATE]/lib/utils/filenamehandler.pri)
include( $$[PRTEMPLATE]/lib/utils/ibb-logo.pri)
include( $$[PRTEMPLATE]/lib/utils/ibtoolbar.pri)
include( $$[PRTEMPLATE]/lib/utils/stdicons.pri)
# include( $$[PRTEMPLATE]/lib/utils/commsocket/commserver.pri)
# include( $$[PRTEMPLATE]/lib/utils/commsocket/commsocket.pri)
# include( $$[PRTEMPLATE]/lib/utils/fileselector/fileselector.pri)
# include( $$[PRTEMPLATE]/lib/utils/filetextstream/filetextstream.pri)
# include( $$[PRTEMPLATE]/lib/utils/history/history.pri)
# include( $$[PRTEMPLATE]/lib/utils/ledicon/ledicon.pri)
# include( $$[PRTEMPLATE]/lib/utils/scrypt/simplecrypt.pri)
# include( $$[PRTEMPLATE]/lib/utils/singleapp/singleapp.pri)
# include( $$[PRTEMPLATE]/lib/websocket/webserver.pri)
# include( $$[PRTEMPLATE]/lib/websocket/websocket.pri)
# include( $$[PRTEMPLATE]/lib/ZConf/zconfclient.pri)
# include( $$[PRTEMPLATE]/lib/ZConf/zconfsrv.pri)
# include( $$[PRTEMPLATE]/qcustomplot-1.3.1/qcustomplot.pri)

TARGET = picoload

QT += serialport

DEFINES +=

HEADERS += \
	mainwindow.h

SOURCES += \
	main.cpp \
	mainwindow.cpp

FORMS += \
	mainwindow.ui

RESOURCES += \
	rsc/rsc.qrc

DISTFILES += \
	.gitignore \
	LICENSE \
	README.md \
	rsc/logo.png \
	rsc/defaults.rc \
	rsc/styles.css

