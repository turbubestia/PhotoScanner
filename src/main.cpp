/*
 * main.cpp
 *
 *  Created on: 22 sep. 2019
 *      Author: claud
 */

#include <QtWidgets/QApplication>

#include "MainWindow.h"

#include "stdio.h"
void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
	Q_UNUSED(type);
	Q_UNUSED(context);

    printf(qUtf8Printable(msg));
    printf("\n");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
	qInstallMessageHandler(messageHandler);

	QApplication app(argc, argv);

	QFile f(":qdarkstyle/style.qss");
	if (!f.exists()) {
	    printf("Unable to set stylesheet, file not found\n");
	} else {
	    f.open(QFile::ReadOnly | QFile::Text);
	    QTextStream ts(&f);
	    app.setStyleSheet(ts.readAll());
	}

	MainWindow  mainWin;
	mainWin.show();

	return app.exec();
}


