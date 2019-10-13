/*
 * MainWindow.cpp
 *
 *  Created on: 22 sep. 2019
 *      Author: claud
 */

#include <QDebug>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QFileSystemModel>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/saturate.hpp>
#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), original(new QLabel), page(new QLabel)
{
	// Resize to a reasonable size
	resize(QGuiApplication::primaryScreen()->availableSize() * 4/5);

	// ************
	// File browser
	// ************

	fileModel = new QFileSystemModel;
	fileModel->setRootPath("D:\\scanner");
	fileModel->setNameFilters({"*.jpg", "*.jpeg"});
	fileModel->setNameFilterDisables(false);

	fileBrowser = new QTreeView();
	fileBrowser->setModel(fileModel);
	fileBrowser->setRootIndex(fileModel->index("D:\\scanner"));
	fileBrowser->setColumnHidden(1, true);
	fileBrowser->setColumnHidden(2, true);
	fileBrowser->setColumnWidth(0, width()*0.2*0.8);

	// *********
	// Page size
	// *********
	gbSize = new QGroupBox("Page size");
	QVBoxLayout *glPageSize = new QVBoxLayout;
	gbSize->setLayout(glPageSize);
	gbSize->setEnabled(false);

	{ // Size configuration
		QGridLayout *widgetLayout = new QGridLayout;

		lbInputImageSize = new QLabel("0 x 0 px");
		widgetLayout->addWidget(new QLabel("Input size:"), 0, 0);
		widgetLayout->addWidget(lbInputImageSize, 0, 1, 1, 2);
		glPageSize->addLayout(widgetLayout);

		rbAspectRatioLocked = new QRadioButton("Locked");
		rbAspectRatioFree = new QRadioButton("Free");
		rbAspectRatioLocked->setChecked(true);
		QButtonGroup *bg1 = new QButtonGroup;
		bg1->addButton(rbAspectRatioLocked);
		bg1->addButton(rbAspectRatioFree);
		widgetLayout->addWidget(new QLabel("Aspect Ratio:"), 1, 0);
		widgetLayout->addWidget(rbAspectRatioLocked, 1, 1);
		widgetLayout->addWidget(rbAspectRatioFree, 1, 2);

		rbPixelLock = new QRadioButton("Pixel");
		rbPaperLock = new QRadioButton("Paper");
		rbPaperLock->setChecked(true);
		QButtonGroup *bg2 = new QButtonGroup;
		bg2->addButton(rbPixelLock);
		bg2->addButton(rbPaperLock);
		widgetLayout->addWidget(new QLabel("Size source:"), 2, 0);
		widgetLayout->addWidget(rbPixelLock, 2, 1);
		widgetLayout->addWidget(rbPaperLock, 2, 2);

		glPageSize->addLayout(widgetLayout);
	}

	{ // Page size combo
		QHBoxLayout *widgetLayout = new QHBoxLayout;
		cbPageSize = new QComboBox;
		cbPageSize->addItem("Original");
		cbPageSize->addItem("Custom");
		cbPageSize->addItem("Letter");
		cbPageSize->addItem("Legal");
		cbPageSize->addItem("A4");
		cbPageSize->setCurrentIndex(2);
		leDpi = new QLineEdit;
		leDpi->setValidator(new QIntValidator(72,600));
		leDpi->setText("150");
		leDpi->setMaximumWidth(50);

		QLabel *label;
		QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Maximum);

		label = new QLabel("Page size:");
		label->setSizePolicy(policy);
		widgetLayout->addWidget(label);
		widgetLayout->addWidget(cbPageSize);

		label = new QLabel("dpi:");
		label->setSizePolicy(policy);
		widgetLayout->addWidget(label);
		widgetLayout->addWidget(leDpi);

		glPageSize->addLayout(widgetLayout);
	}

	{ // Set sizes
		QGridLayout *widgetLayout = new QGridLayout;

		lePixelWidth = new QLineEdit;
		lePixelWidth->setValidator(new QIntValidator(100,10000));
		lePixelHeight = new QLineEdit;
		lePixelHeight->setValidator(new QIntValidator(100,10000));
		widgetLayout->addWidget(new QLabel("Pixel size:"), 0, 0);
		widgetLayout->addWidget(lePixelWidth, 0, 1);
		widgetLayout->addWidget(lePixelHeight, 0, 2);

		lePhysicalWidth = new QLineEdit;
		lePhysicalHeight = new QLineEdit;
		cbUnits = new QComboBox;
		cbUnits->addItem("mm");
		cbUnits->addItem("cm");
		cbUnits->addItem("in");
		cbUnits->setCurrentIndex(2);
		widgetLayout->addWidget(new QLabel("Paper size:"), 1, 0);
		widgetLayout->addWidget(lePhysicalWidth, 1, 1);
		widgetLayout->addWidget(lePhysicalHeight, 1, 2);
		widgetLayout->addWidget(cbUnits, 1, 3);

		glPageSize->addLayout(widgetLayout);
	}

	// ***************
	// Page adjustment
	// ***************
	gbAdjust = new QGroupBox;
	QGridLayout *glControl = new QGridLayout;

	slWhite = new QSlider(Qt::Horizontal);
	slBlack = new QSlider(Qt::Horizontal);
	sbWhite = new QSpinBox;
	sbBlack = new QSpinBox;

	slWhite->setMaximum(255);
	slWhite->setMinimum(128);
	sbWhite->setMaximum(255);
	sbWhite->setMinimum(128);
	slBlack->setMaximum(127);
	slBlack->setMinimum(0);
	sbBlack->setMaximum(127);
	sbBlack->setMinimum(0);

	glControl->addWidget(new QLabel("White"),0,0);
	glControl->addWidget(slWhite,0,1);
	glControl->addWidget(sbWhite,0,2);
	glControl->addWidget(new QLabel("Black"),1,0);
	glControl->addWidget(slBlack,1,1);
	glControl->addWidget(sbBlack,1,2);

	gbAdjust->setLayout(glControl);

	// ***********
	// Save button
	// ***********
	pbSave = new QPushButton("Save");

	// **************
	// Control Layout
	// **************
	QVBoxLayout *controlLayout = new QVBoxLayout;
	controlLayout->addWidget(fileBrowser);
	controlLayout->addWidget(gbSize);
	controlLayout->addWidget(gbAdjust);
	controlLayout->addWidget(pbSave);

	QWidget *control = new QWidget;
	control->setLayout(controlLayout);

	// Setup the image preview panel
	original->setBackgroundRole(QPalette::Base);
	original->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	original->setScaledContents(true);

	page->setBackgroundRole(QPalette::Base);
	page->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	page->setScaledContents(true);

	QHBoxLayout *viewerLayout = new QHBoxLayout;
	viewerLayout->addWidget(original);
	viewerLayout->addWidget(page);
	QWidget *viewer = new QWidget;
	viewer->setLayout(viewerLayout);

	// Join them with a splitter
	QSplitter *splitter = new QSplitter(this);
	splitter->addWidget(control);
	splitter->addWidget(viewer);

	setCentralWidget(splitter);

	// Allow more space for the images
	double sr = 0.20;
	int w1 = width()*sr;
	int w2 = width()*(1-sr);
	splitter->setSizes(QList<int>({w1,w2}));

	connect(fileBrowser, &QTreeView::clicked, this, &MainWindow::clicked);

	connect(rbPixelLock, &QRadioButton::clicked, this, &MainWindow::SizeSourceChanged);
	connect(rbPaperLock, &QRadioButton::clicked, this, &MainWindow::SizeSourceChanged);

	connect(cbPageSize, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::pageSizeSelected); //@suppress("Invalid arguments")
	connect(leDpi, &QLineEdit::textEdited, this, &MainWindow::pageDpiChanged);

	connect(lePixelWidth, &QLineEdit::textEdited, this, &MainWindow::pagePixelSizeChanged);
	connect(lePixelHeight, &QLineEdit::textEdited, this, &MainWindow::pagePixelSizeChanged);
	connect(lePhysicalWidth, &QLineEdit::textEdited, this, &MainWindow::pagePaperSizedChanged);
	connect(lePhysicalHeight, &QLineEdit::textEdited, this, &MainWindow::pagePaperSizedChanged);

	connect(slWhite, &QSlider::valueChanged, sbWhite, &QSpinBox::setValue);
	connect(sbWhite, QOverload<int>::of(&QSpinBox::valueChanged), slWhite, &QSlider::setValue); //@suppress("Invalid arguments")
	connect(slBlack, &QSlider::valueChanged, sbBlack, &QSpinBox::setValue);
	connect(sbBlack, QOverload<int>::of(&QSpinBox::valueChanged), slBlack, &QSlider::setValue); //@suppress("Invalid arguments")

	sbWhite->setValue(235);
	sbBlack->setValue(20);

	connect(sbWhite, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::controlChanged); //@suppress("Invalid arguments")
	connect(sbBlack, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::controlChanged); //@suppress("Invalid arguments")

	connect(pbSave, &QPushButton::clicked, this, &MainWindow::save);
}

void MainWindow::clicked(const QModelIndex &index) {
	fileName = fileModel->filePath(index);
	bool fromScanned = fileName.contains("_scanned");

	if (fromScanned || fileModel->isDir(index)) {
		original->setPixmap(QPixmap());
		page->setPixmap(QPixmap());
		imgPageOriginal = QImage();
		fileName = QString();

		gbSize->setEnabled(false);
		gbAdjust->setEnabled(false);

	} else {
		// The the opened image
		QImageReader reader(fileName);
		reader.setAutoTransform(true);
		const QImage inImage = reader.read();
		original->setPixmap(QPixmap::fromImage(inImage));

		lbInputImageSize->setText(QString("%1 x %2 px").arg(inImage.width()).arg(inImage.height()));

		// Scan it
		imgPageOriginal = scan(inImage);

		pageSizeSelected();

		gbSize->setEnabled(true);
		gbAdjust->setEnabled(true);
	}
}

void MainWindow::SizeSourceChanged() {
	if (cbPageSize->currentIndex() == 1) {
		if (rbPixelLock->isChecked()) {
			lePixelWidth->setEnabled(true);
			lePixelHeight->setEnabled(true);
			lePhysicalWidth->setEnabled(false);
			lePhysicalHeight->setEnabled(false);
		} else {
			lePixelWidth->setEnabled(false);
			lePixelHeight->setEnabled(false);
			lePhysicalWidth->setEnabled(true);
			lePhysicalHeight->setEnabled(true);
		}
	} else {
		lePixelWidth->setEnabled(false);
		lePixelHeight->setEnabled(false);
		lePhysicalWidth->setEnabled(false);
		lePhysicalHeight->setEnabled(false);
	}
}

void MainWindow::pageSizeSelected() {
	SizeSourceChanged();

	if (cbPageSize->currentIndex() == 1) {
		rbAspectRatioLocked->setEnabled(true);
		rbAspectRatioFree->setEnabled(true);
		rbPixelLock->setEnabled(true);
		rbPaperLock->setEnabled(true);
		cbUnits->setEnabled(true);
	} else {
		rbAspectRatioLocked->setEnabled(false);
		rbAspectRatioFree->setEnabled(false);
		rbPixelLock->setEnabled(false);
		rbPaperLock->setEnabled(false);
		cbUnits->setEnabled(false);
	}

	switch (cbPageSize->currentIndex()) {
		case 0:
			lePixelWidth->setText(QString::number(imgPageOriginal.width()));
			lePixelHeight->setText(QString::number(imgPageOriginal.height()));
			pagePixelSizeChanged();
			break;

		case 1:
			QMetaObject::invokeMethod(lePixelWidth,"textEdited", Qt::DirectConnection, Q_ARG(QString, ""));
			break;

		case 2:
			lePhysicalWidth->setText("8.5");
			lePhysicalHeight->setText("11.0");
			cbUnits->setCurrentIndex(2);
			pagePaperSizedChanged();
			break;

		case 3:
			lePhysicalWidth->setText("8.5");
			lePhysicalHeight->setText("14.0");
			cbUnits->setCurrentIndex(2);
			pagePaperSizedChanged();
			break;

		case 4:
			lePhysicalWidth->setText("210");
			lePhysicalHeight->setText("297");
			cbUnits->setCurrentIndex(0);
			pagePaperSizedChanged();
			break;
	}
}

void MainWindow::pagePixelSizeChanged() {

	double dpi = leDpi->text().toDouble();
	int pw = lePixelWidth->text().toInt();
	int ph = lePixelHeight->text().toInt();

	if(rbAspectRatioLocked->isChecked()) {
		double ar = (double) imgPageOriginal.width() / (double) imgPageOriginal.height();
		QLineEdit *obj = qobject_cast<QLineEdit*>(sender());
		if (obj == lePixelWidth) {
			ph = (double) pw / ar;
			lePixelHeight->setText(QString::number(ph));
		} else if(obj == lePixelHeight) {
			pw = (double) ph * ar;
			lePixelWidth->setText(QString::number(pw));
		}
	}

	double w = (double) pw / dpi;
	double h = (double) ph / dpi;

	lePhysicalWidth->setText(QString::number(w, 'f', 1));
	lePhysicalHeight->setText(QString::number(h, 'f', 1));

	sizeChanged();
}

void MainWindow::pagePaperSizedChanged() {
	double dpi = leDpi->text().toDouble();
	double pw = lePhysicalWidth->text().toDouble();
	double ph = lePhysicalHeight->text().toDouble();
	double units = 1;

	if(cbUnits->currentIndex() == 0) {
		units = 25.4;
	} else if(cbUnits->currentIndex() == 1) {
		units = 2.5;
	}

	if(rbAspectRatioLocked->isChecked()) {
		double ar = (double) imgPageOriginal.width() / (double) imgPageOriginal.height();
		QLineEdit *obj = qobject_cast<QLineEdit*>(sender());
		if (obj == lePhysicalWidth) {
			ph = pw / ar;
			lePhysicalHeight->setText(QString::number(ph, 'f', 1));
		} else if(obj == lePhysicalHeight) {
			pw = ph * ar;
			lePhysicalWidth->setText(QString::number(pw, 'f', 1));
		}
	}

	int w = pw * dpi / units;
	int h = ph * dpi / units;

	lePixelWidth->setText(QString::number(w));
	lePixelHeight->setText(QString::number(h));

	sizeChanged();
}

void MainWindow::pageDpiChanged() {
	if (cbPageSize->currentIndex() == 0) {
		pagePixelSizeChanged();
	} else if (cbPageSize->currentIndex() == 1) {
		if (rbPixelLock->isChecked()) {
			pagePixelSizeChanged();
		} else {
			pagePaperSizedChanged();
		}
	} else if (cbPageSize->currentIndex() > 1) {
		pagePaperSizedChanged();
	}
}

void MainWindow::sizeChanged() {
	scale();
	adjust();
}

void MainWindow::controlChanged() {
	adjust();
}

void MainWindow::save() {
	if (imgPageProcessed.isNull() || fileName.isEmpty()) {
		return;
	}

	QFileInfo fileInfo(fileName);
	QString outputFile = QString("%1/%2_scanned.jpg").arg(fileInfo.path()).arg(fileInfo.baseName());
	QImageWriter writer(outputFile);
	writer.setQuality(90);
	writer.write(imgPageProcessed);
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

QImage MainWindow::scan(const QImage &inImage) {
	// Process the image with openCV
	cv::Mat inMat = QImageToCvMat(inImage);
	// Intermediate working copies
	cv::Mat imgA;
	cv::Mat imgB;

	// Re-scale to process it faster
	int height = 800;
	double ratio = (double) height / (double) inMat.rows;
	int width = inMat.cols * ratio;
	cv::resize(inMat, imgA, cv::Size(width, height));

	// Border detection
	cv::cvtColor(imgA, imgB, cv::COLOR_BGRA2GRAY);

	// Create a border with the average color of top line
	int avg = 0;
	for(int x = 0; x < imgB.cols; x++) {
		avg += imgB.data[x];
	}
	avg /= imgB.cols;
	cv::copyMakeBorder(imgB, imgA, 0,0,5,5, cv::BORDER_CONSTANT, cv::Scalar(avg));

	cv::bilateralFilter(imgA, imgB, 5, 35, 95);
	cv::adaptiveThreshold(imgB, imgA, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 91, 4);
	cv::medianBlur(imgA, imgB, 9);
	cv::Canny(imgB, imgA, 200, 250);

	// Find contour
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(imgA, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

	// Filter the contour to that that is the biggest closed and convex
	double MAX_CONTOUR_AREA = (width - 10) * (height - 10);
	double maxAreaFound = MAX_CONTOUR_AREA * 0.5;

	std::vector<cv::Point> page = {{5,5}, {5, height-5}, {width-5, height-5}, {width-5, 5}};

	for(int i = 0; i < (int)contours.size(); i++) {
		std::vector<cv::Point> cnt = contours.at(i);
		std::vector<cv::Point> approx_cnt;

		double perimeter = cv::arcLength(cnt, true);
		cv::approxPolyDP(cnt, approx_cnt, 0.03 * perimeter, true);
		double area = cv::contourArea(approx_cnt);

		bool rule = approx_cnt.size() == 4;
		rule = rule && cv::isContourConvex(approx_cnt);
		rule = rule && maxAreaFound < area;
		rule = rule && area < MAX_CONTOUR_AREA;

		if (rule) {
			maxAreaFound = area;
			page = approx_cnt;
		}
	}

	// Sort contour corners
	int topLeft = 0;
	int bottomLeft = 0;
	int bottomRight = 0;
	int topRight = 0;

	double minDiff = 1e9, maxDiff = -1e9;
	double minSum = 1e9, maxSum = -1e9;
	for(int i=0; i < (int)page.size(); i++) {
		double diff = page.at(i).x - page.at(i).y;
		double sum = page.at(i).x + page.at(i).y;
		if(diff > maxDiff) { maxDiff = diff; topRight = i; }
		if(diff < minDiff) { minDiff = diff; bottomLeft = i; }
		if(sum > maxSum) { maxSum = sum; bottomRight = i; }
		if(sum < minSum) { minSum = sum; topLeft = i; }
	}

	// Map point back to the original image size
	std::vector<cv::Point> aux;
	aux.push_back((page.at(topLeft) + cv::Point(2-5,2)) / ratio);
	aux.push_back((page.at(bottomLeft) + cv::Point(2-5,-2)) / ratio);
	aux.push_back((page.at(bottomRight) + cv::Point(-2-5,-2)) / ratio);
	aux.push_back((page.at(topRight) + cv::Point(-2-5,2)) / ratio);
	page = aux;

	// Draw contour in the original image
	contours.clear();
	contours.push_back(page);
	cv::drawContours(inMat, contours, -1, cv::Scalar(255,255,255), 5);

	// Mean page size
	double h1 = fabs(page.at(0).y - page.at(1).y);
	double h2 = fabs(page.at(2).y - page.at(3).y);
	double w1 = fabs(page.at(1).x - page.at(2).x);
	double w2 = fabs(page.at(0).x - page.at(3).x);
	int pageHeight = (h1 + h2)/2;
	int pageWidth = (w1 + w2)/2;

	// Source and target page points
	std::vector<cv::Point2f> sPoints;
	sPoints.push_back(cv::Point2f(page.at(0).x, page.at(0).y));
	sPoints.push_back(cv::Point2f(page.at(1).x, page.at(1).y));
	sPoints.push_back(cv::Point2f(page.at(2).x, page.at(2).y));
	sPoints.push_back(cv::Point2f(page.at(3).x, page.at(3).y));

	std::vector<cv::Point2f> tPoints;
	tPoints.push_back(cv::Point2f(0,0));
	tPoints.push_back(cv::Point2f(0,pageHeight));
	tPoints.push_back(cv::Point2f(pageWidth,pageHeight));
	tPoints.push_back(cv::Point2f(pageWidth,0));

	// Get perspective matrix
	cv::Mat M = cv::getPerspectiveTransform(sPoints, tPoints);
	// And apply it
	cv::Mat outMat;
	cv::warpPerspective(inMat, imgA, M, cv::Size(pageWidth, pageHeight));
	// Smooth by preserve edges
	cv::bilateralFilter(imgA, imgB, 5, 35, 95);

	return cvMatToQImage(imgB);
}

void MainWindow::adjust() {
	if (imgPageOriginal.isNull() || fileName.isEmpty()) {
		imgPageProcessed = QImage();
		return;
	}

	imgPageProcessed = imgPageScalled.copy();

	// Improve brightness and contrast
	int indentRight = sbWhite->value();
	int indentLeft = sbBlack->value();
	int xdist = indentRight - indentLeft; // indent as index from 0 to 256, for the above picture: indentRight: 235, indentLeft: 15
	double alpha = 255.0 / xdist;
	int beta = (int) -(indentLeft* alpha);

	uchar *data = imgPageProcessed.bits();
	for(int i = 0; i < imgPageProcessed.sizeInBytes(); i++) {
		data[i] = cv::saturate_cast<uchar>( alpha*( data[i] ) + beta );
	}

	page->setPixmap(QPixmap::fromImage(imgPageProcessed));
}

void MainWindow::scale() {
	if (imgPageOriginal.isNull() || fileName.isEmpty()) {
		return;
	}

	int w = lePixelWidth->text().toInt();
	int h = lePixelHeight->text().toInt();

	imgPageScalled = imgPageOriginal.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}


