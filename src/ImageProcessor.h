#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QtCore>
#include <QtGui>

#include <opencv4/opencv2/opencv.hpp>

class ImageProcessor
{
    public:
        enum SizePolicy {SizeSource, SizeCustom};

        ImageProcessor();

        void setSourceImage(QString filename);
        QImage getSourceImage();
        QImage getScannedImage();

        void setRoi(QRect roi);
        QRect getRoi();

        void setSizePolicy(ImageProcessor::SizePolicy policy);
        void setSize(double w, double h);

        QRect findPage();
        void scanPage();

    private:
        QString filename;

        cv::Mat source;
        cv::Mat scanned;

        std::vector<cv::Point> page;

        SizePolicy sizePolicy;

        double customWidth;
        double customHeight;
};

#endif // IMAGEPROCESSOR_H
