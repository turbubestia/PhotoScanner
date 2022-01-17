/*
 * Utils.cpp
 *
 *  Created on: 18 abr. 2020
 *      Author: claud
 */

#include "Utils.h"

QImage cvMatToQImage(const cv::Mat &inMat) {
	switch(inMat.type()) {
		case CV_8UC4: {
			QImage outImage(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_ARGB32);
			return outImage.rgbSwapped();
		}

		case CV_8UC3: {
			QImage outImage(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_RGB888);
			return outImage.rgbSwapped();
		}

		default:
			return QImage();
	}
}

cv::Mat QImageToCvMat(const QImage &inImage) {
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
			qDebug() << "Unsupported format";
			return cv::Mat();
	}
}
