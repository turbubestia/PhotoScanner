/*
 * main.cpp
 *
 *  Created on: 22 sep. 2019
 *      Author: claud
 */

#include <QtWidgets/QApplication>

#include "MainWindow.h"
#include "ImageProcessor.h"

int main(int argc, char *argv[])
{
	printf("Starting application\n");

	QApplication app(argc, argv);

#if 0
	QFile f(":qdarkstyle/style.qss");
	if (!f.exists()) {
	    printf("Unable to set stylesheet, file not found\n");
	} else {
	    f.open(QFile::ReadOnly | QFile::Text);
	    QTextStream ts(&f);
	    app.setStyleSheet(ts.readAll());
	}
#endif

	MainWindow  mainWin;
	mainWin.show();

	return app.exec();
}


