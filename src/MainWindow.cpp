/*
 * MainWindow.cpp
 *
 *  Created on: 22 sep. 2019
 *      Author: claud
 */

#include <QtWidgets/QtWidgets>
#include <opencv/cv.hpp>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), photoview(new QLabel)
{

	photoview->setBackgroundRole(QPalette::Base);
	photoview->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	photoview->setScaledContents(true);

	setCentralWidget(photoview);
	QImageReader reader("./src/test.jpg");
	reader.setAutoTransform(true);
	const QImage newImage = reader.read();

	// Process the image with openCV
	cv::Mat inMat = QImageToCvMat(newImage);

	double height = 800;
	double ratio = height / inMat.rows;
	double width = inMat.cols * ratio;

	//cv::Mat imgA((int)(height), (int)(width), inMat.type());
	cv::Mat imgA;
	cv::Mat imgB;

	cv::resize(inMat, imgA, cv::Size(height, width));
	cv::cvtColor(imgA, imgB, cv::COLOR_BGRA2GRAY);

	cv::bilateralFilter(imgB, imgA, 9, 75, 75);
	cv::adaptiveThreshold(imgA, imgB, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 115, 4);
	cv::medianBlur(imgB, imgA, 11);
	cv::copyMakeBorder(imgA, imgB, 5, 5, 5, 5, cv::BORDER_REPLICATE);
	cv::Canny(imgB, imgA, 200, 250);

	// Find contour
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;

	cv::findContours(imgA, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
	cv::cvtColor(imgA, imgB, cv::COLOR_GRAY2BGRA);

	// Display the image in Qt
	QImage outImage = cvMatToQImage(imgB);

	QSize imgSize = outImage.size();
	double imgWidth = outImage.width();
	double imgHeight = outImage.height();

	QSize maxSize = QGuiApplication::primaryScreen()->availableSize() * 4/5;
	double maxWidth = maxSize.width();
	double maxHeight = maxSize.height();

	ratio = 1.0;
	if(imgWidth > imgHeight) {
		ratio = maxWidth/imgWidth;
	} else {
		ratio = maxHeight/imgHeight;
	}
	imgSize *= ratio;

	photoview->setPixmap(QPixmap::fromImage(outImage));
	resize(imgSize);
}


cv::Mat MainWindow::QImageToCvMat(const QImage &inImage) {
	switch (inImage.format()) {
		case QImage::Format_ARGB32:
		case QImage::Format_ARGB32_Premultiplied: {
			cv::Mat outMat(inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), static_cast<size_t>(inImage.bytesPerLine()));
			return outMat.clone();
		}

		case QImage::Format_RGB32: {
			cv::Mat mat(inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()),
					static_cast<size_t>(inImage.bytesPerLine()));
			cv::Mat outMat;
			cv::cvtColor(mat, outMat, cv::COLOR_BGRA2BGR);
			return outMat;
		}

		case QImage::Format_RGB888: {
			QImage swapped = inImage.rgbSwapped();
			cv::Mat outMat(swapped.height(), swapped.width(), CV_8UC3,const_cast<uchar*>(swapped.bits()),
					static_cast<size_t>(swapped.bytesPerLine()));
			return outMat.clone();
		}

		default:
			return cv::Mat();
	}
}

QImage MainWindow::cvMatToQImage(const cv::Mat &inMat) {
	switch(inMat.type()) {
		case CV_8UC4: {
			QImage outImage(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_ARGB32);
			return outImage;
		}

		case CV_8UC3: {
			QImage outImage(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_RGB888);
			return outImage;
		}

		default:
			return QImage();
	}
}

