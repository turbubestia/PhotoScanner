/*
 * MainWindow.h
 *
 *  Created on: 22 sep. 2019
 *      Author: claud
 */

#ifndef SRC_MAINWINDOW_H_
#define SRC_MAINWINDOW_H_

#include <QtWidgets/QtWidgets>
#include <opencv/cv.hpp>

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);

private:
	cv::Mat QImageToCvMat(const QImage &inImage);
	QImage cvMatToQImage(const cv::Mat &inMat);


private:
	QLabel *photoview;

};



#endif /* SRC_MAINWINDOW_H_ */
