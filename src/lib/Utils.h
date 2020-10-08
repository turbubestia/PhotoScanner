/*
 * Utils.h
 *
 *  Created on: 18 abr. 2020
 *      Author: claud
 */

#ifndef SRC_UTILS_H_
#define SRC_UTILS_H_

#include <QtWidgets/QtWidgets>
#include <opencv4/opencv2/opencv.hpp>

// OpenCV Interface
QImage cvMatToQImage(const cv::Mat &inMat);
cv::Mat QImageToCvMat(const QImage &inImage);


#endif /* SRC_UTILS_H_ */
