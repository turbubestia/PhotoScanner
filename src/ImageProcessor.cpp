
#include <exiv2/exiv2.hpp>

#include "ImageProcessor.h"

ImageProcessor::ImageProcessor()
    : sizePolicy(SizeSource), customWidth(0), customHeight(0)
{

}

void ImageProcessor::setSourceImage(QString filename)
{
    this->filename = filename;

    source = cv::imread(filename.toStdString(), cv::IMREAD_COLOR);
}

QImage ImageProcessor::getSourceImage()
{
    return QImage(source.data, source.cols, source.rows, source.step, QImage::Format_RGB888);
}

QImage ImageProcessor::getScannedImage()
{
    if (!scanned.empty()) {
        return QImage(scanned.data, scanned.cols, scanned.rows, scanned.step, QImage::Format_RGB888);
    } else {
        return QImage();
    }
}

void ImageProcessor::setRoi(QRect roi)
{
    page.clear();
    page.push_back(cv::Point(roi.topLeft().x(), roi.topLeft().y()));
    page.push_back(cv::Point(roi.bottomLeft().x(), roi.bottomLeft().y()));
    page.push_back(cv::Point(roi.bottomRight().x(), roi.bottomRight().y()));
    page.push_back(cv::Point(roi.topRight().x(), roi.topRight().y()));
}

QRect ImageProcessor::getRoi()
{
    int left = page.at(0).x;
    int top = page.at(0).y;
    int width = page.at(2).x - page.at(0).x;
    int height = page.at(1).y - page.at(0).y;

    return QRect(left,top,width,height);
}

void ImageProcessor::setSizePolicy(ImageProcessor::SizePolicy policy)
{
    sizePolicy = policy;
}

void ImageProcessor::setSize(double w, double h)
{
    customWidth = w;
    customHeight = h;
}

QRect ImageProcessor::findPage()
{
    qDebug() << "ImageProcessor::findPage()";

    // Intermediate working copies
    cv::Mat img0;
    cv::Mat imgA;
    cv::Mat imgB;

    // Re-scale to process it faster
    int height = 800;
    int width = 800;
    double ratio;

    if (source.cols > source.rows) {
        ratio = (double) width / (double) source.cols;
        height = source.rows * ratio;
    } else {
        ratio = (double) height / (double) source.rows;
        width = source.cols * ratio;
    }

    int border = 10;

    cv::resize(source, img0, cv::Size(width,height));
    cv::cvtColor(img0,imgA, cv::COLOR_BGR2GRAY);
    cv::copyMakeBorder(imgA, imgB, 0,0,border,border, cv::BORDER_CONSTANT, cv::mean(imgA.row(0)));
    cv::GaussianBlur(imgB, imgA, cv::Size(7,7), 0);
    cv::threshold(imgA, imgB, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
    cv::Canny(imgB, imgA, 200, 250, 3, true);

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(imgA, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    // Filter the contour to that that is the biggest closed and convex
    double MAX_CONTOUR_AREA = (width - border) * (height - border);
    double maxAreaFound = MAX_CONTOUR_AREA * 0.25;

    std::vector<cv::Point> smallPage = {{border,border}, {border, height-2*border}, {width-2*border, height-2*border}, {width-2*border, border}};

    for(int i = 0; i < (int)contours.size(); i++) {
        std::vector<cv::Point> contour = contours.at(i);
        std::vector<cv::Point> approximatedContour;

        double perimeter = cv::arcLength(contour, true);
        cv::approxPolyDP(contour, approximatedContour, 0.03 * perimeter, true);
        double area = cv::contourArea(approximatedContour);

        bool isPageAccepted = approximatedContour.size() == 4;
        isPageAccepted = isPageAccepted && cv::isContourConvex(approximatedContour);
        isPageAccepted = isPageAccepted && maxAreaFound < area;
        isPageAccepted = isPageAccepted && area < MAX_CONTOUR_AREA;

        if (isPageAccepted) {
            maxAreaFound = area;
            smallPage = approximatedContour;
        }
    }

    // Sort contour corners
    int topLeft = 0;
    int bottomLeft = 0;
    int bottomRight = 0;
    int topRight = 0;

    double minDiff = 1e9, maxDiff = -1e9;
    double minSum = 1e9, maxSum = -1e9;
    for(int i=0; i < (int)smallPage.size(); i++) {
        double diff = smallPage.at(i).x - smallPage.at(i).y;
        double sum = smallPage.at(i).x + smallPage.at(i).y;
        if(diff > maxDiff) { maxDiff = diff; topRight = i; }
        if(diff < minDiff) { minDiff = diff; bottomLeft = i; }
        if(sum > maxSum) { maxSum = sum; bottomRight = i; }
        if(sum < minSum) { minSum = sum; topLeft = i; }
    }

    // Map point back to the original image size

    page.clear();
    page.push_back((smallPage.at(topLeft) + cv::Point(2-border,2)) / ratio);
    page.push_back((smallPage.at(bottomLeft) + cv::Point(2-border,-2)) / ratio);
    page.push_back((smallPage.at(bottomRight) + cv::Point(-2-border,-2)) / ratio);
    page.push_back((smallPage.at(topRight) + cv::Point(-2-border,2)) / ratio);

    int left = page.at(0).x;
    int top = page.at(0).y;
    width = page.at(2).x - page.at(0).x;
    height = page.at(1).y - page.at(0).y;

    return QRect(left,top,width,height);
}

void ImageProcessor::scanPage()
{
    qDebug() << "MainWindow::scanPage()";

    int pageHeight;
    int pageWidth;

    if (sizePolicy == SizeSource) {
        // Mean page size
        double h1 = fabs(page.at(0).y - page.at(1).y);
        double h2 = fabs(page.at(2).y - page.at(3).y);
        double w1 = fabs(page.at(1).x - page.at(2).x);
        double w2 = fabs(page.at(0).x - page.at(3).x);
        pageHeight = (h1 + h2)/2;
        pageWidth = (w1 + w2)/2;
    }
    else {
        pageHeight = customHeight;
        pageWidth = customWidth;
    }

    std::vector<cv::Point2f> target;
    target.push_back(cv::Point2f(0,0));
    target.push_back(cv::Point2f(0,pageHeight));
    target.push_back(cv::Point2f(pageWidth,pageHeight));
    target.push_back(cv::Point2f(pageWidth,0));

    cv::Mat M = cv::getPerspectiveTransform(page, target);
    cv::warpPerspective(source, scanned, M, cv::Size(pageWidth, pageHeight));
}
