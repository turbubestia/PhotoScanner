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
	: QMainWindow(parent)
{
	// Resize to a reasonable size
	resize(QGuiApplication::primaryScreen()->availableSize() * 4/5);

	createActions();

	createControlObject();
	createLayout();

	createConnections();

	sbWhite->setValue(235);
	sbBlack->setValue(20);
}

void MainWindow::about() {

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

	//page->setPixmap(QPixmap::fromImage(imgPageProcessed));
	viewerPage->setPixmap(QPixmap::fromImage(imgPageProcessed));
}

void MainWindow::adjustImage() {
	adjust();
}

void MainWindow::createActions() {
	QMenu *fileMenu = menuBar()->addMenu("Menu");
	QAction *rootAction = new QAction("Root folder", this);
	fileMenu->addAction(rootAction);
	fileMenu->addSeparator();
	QAction *aboutAction = new QAction("About", this);
	fileMenu->addAction(aboutAction);

	connect(rootAction, &QAction::triggered, this, &MainWindow::setRootFolder);
	connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
}

void MainWindow::createConnections() {
	connect(fileBrowser, &QTreeView::clicked, this, &MainWindow::selectFile);

	connect(rbPixelSizeMode, &QRadioButton::clicked, this, &MainWindow::updatePixelPaperMode);
	connect(rbPaperSizeMode, &QRadioButton::clicked, this, &MainWindow::updatePixelPaperMode);

	connect(cbPageSizeTemplate, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updatePageSizeTemplate); //@suppress("Invalid arguments")
	connect(leDpi, &QLineEdit::textEdited, this, &MainWindow::updateDpi);

	connect(lePixelWidth, &QLineEdit::textEdited, this, &MainWindow::updatePixelSize);
	connect(lePixelHeight, &QLineEdit::textEdited, this, &MainWindow::updatePixelSize);
	connect(lePaperWidth, &QLineEdit::textEdited, this, &MainWindow::updatePaperSize);
	connect(lePaperHeight, &QLineEdit::textEdited, this, &MainWindow::updatePaperSize);

	connect(slWhite, &QSlider::valueChanged, sbWhite, &QSpinBox::setValue);
	connect(sbWhite, QOverload<int>::of(&QSpinBox::valueChanged), slWhite, &QSlider::setValue); //@suppress("Invalid arguments")
	connect(slBlack, &QSlider::valueChanged, sbBlack, &QSpinBox::setValue);
	connect(sbBlack, QOverload<int>::of(&QSpinBox::valueChanged), slBlack, &QSlider::setValue); //@suppress("Invalid arguments")

	connect(sbWhite, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::adjustImage); //@suppress("Invalid arguments")
	connect(sbBlack, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::adjustImage); //@suppress("Invalid arguments")

	connect(pbSave, &QPushButton::clicked, this, &MainWindow::save);
}

void MainWindow::createControlObject() {

	// ------------------------------------------------------------------------
	// File browser
	QString rootFolder = readRootFolder();

	fileModel = new QFileSystemModel;
	fileModel->setRootPath(rootFolder);
	fileModel->setNameFilters({"*.jpg", "*.jpeg"});
	fileModel->setNameFilterDisables(false);

	fileBrowser = new QTreeView();
	fileBrowser->setModel(fileModel);
	fileBrowser->setRootIndex(fileModel->index(rootFolder));
	fileBrowser->setColumnHidden(1, true);
	fileBrowser->setColumnHidden(2, true);
	fileBrowser->setColumnWidth(0, width()*0.2*0.8);

	// ------------------------------------------------------------------------
	// Both original and process page image
	viewerOriginal = new PageViewer;
	viewerOriginal->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	viewerPage = new PageViewer;
	viewerPage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

	// ------------------------------------------------------------------------
	// Page size controls
	gbSize = new QGroupBox("Page size");

	lbInputImageSize = new QLabel("0 x 0 px");

	rbAspectRatioLock = new QRadioButton("Locked");
	rbAspectRatioFree = new QRadioButton("Free");
	rbAspectRatioLock->setChecked(true);

	rbPixelSizeMode = new QRadioButton("Pixel");
	rbPaperSizeMode = new QRadioButton("Paper");
	rbPaperSizeMode->setChecked(true);

	// The button group allows to group mutually exclusive radio buttons. Without doing this
	// the group box put all of them in the same mutually exclusive group.
	QButtonGroup *bg1 = new QButtonGroup;
	bg1->addButton(rbAspectRatioLock);
	bg1->addButton(rbAspectRatioFree);

	QButtonGroup *bg2 = new QButtonGroup;
	bg2->addButton(rbPixelSizeMode);
	bg2->addButton(rbPaperSizeMode);

	cbPageSizeTemplate = new QComboBox;
	cbPageSizeTemplate->addItem("Original");
	cbPageSizeTemplate->addItem("Custom");
	cbPageSizeTemplate->addItem("Letter");
	cbPageSizeTemplate->addItem("Legal");
	cbPageSizeTemplate->addItem("A4");
	cbPageSizeTemplate->setCurrentIndex(2);

	leDpi = new QLineEdit;
	leDpi->setValidator(new QIntValidator(72,600));
	leDpi->setText("150");
	leDpi->setMaximumWidth(50);

	lePixelWidth = new QLineEdit;
	lePixelWidth->setValidator(new QIntValidator(100,10000));
	lePixelHeight = new QLineEdit;
	lePixelHeight->setValidator(new QIntValidator(100,10000));

	lePaperWidth = new QLineEdit;
	lePaperHeight = new QLineEdit;

	cbPaperUnits = new QComboBox;
	cbPaperUnits->addItem("mm");
	cbPaperUnits->addItem("cm");
	cbPaperUnits->addItem("in");
	cbPaperUnits->setCurrentIndex(2);

	// ------------------------------------------------------------------------
	// Page color adjustment
	gbAdjust = new QGroupBox;

	slWhite = new QSlider(Qt::Horizontal);
	sbWhite = new QSpinBox;
	slWhite->setMaximum(255);
	slWhite->setMinimum(128);
	sbWhite->setMaximum(255);
	sbWhite->setMinimum(128);

	slBlack = new QSlider(Qt::Horizontal);
	sbBlack = new QSpinBox;
	slBlack->setMaximum(127);
	slBlack->setMinimum(0);
	sbBlack->setMaximum(127);
	sbBlack->setMinimum(0);

	// ---------------------------------------------
	// Button used to save the processed image
	pbSave = new QPushButton("Save");
}

void MainWindow::createLayout() {
	// ------------------------------------------------------------------------
	// Page size group box
	QGridLayout *gridlayout1 = new QGridLayout;
	gridlayout1->addWidget(new QLabel("Input size:"), 0, 0);
	gridlayout1->addWidget(lbInputImageSize, 0, 1, 1, 2);
	gridlayout1->addWidget(new QLabel("Aspect Ratio:"), 1, 0);
	gridlayout1->addWidget(rbAspectRatioLock, 1, 1);
	gridlayout1->addWidget(rbAspectRatioFree, 1, 2);
	gridlayout1->addWidget(new QLabel("Size source:"), 2, 0);
	gridlayout1->addWidget(rbPixelSizeMode, 2, 1);
	gridlayout1->addWidget(rbPaperSizeMode, 2, 2);

	QLabel *label1;
	QSizePolicy policy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	label1 = new QLabel("Page size:");
	label1->setSizePolicy(policy);
	gridlayout1->addWidget(label1, 3, 0);
	QHBoxLayout *hboxLayout1 = new QHBoxLayout;
	hboxLayout1->addWidget(cbPageSizeTemplate);
	label1 = new QLabel("dpi:");
	label1->setSizePolicy(policy);
	hboxLayout1->addWidget(label1);
	hboxLayout1->addWidget(leDpi);
	gridlayout1->addLayout(hboxLayout1, 3, 1, 1, 3);

	gridlayout1->addWidget(new QLabel("Pixel size:"), 4, 0);
	gridlayout1->addWidget(lePixelWidth, 4, 1);
	gridlayout1->addWidget(lePixelHeight, 4, 2);
	gridlayout1->addWidget(new QLabel("Paper size:"), 5, 0);
	gridlayout1->addWidget(lePaperWidth, 5, 1);
	gridlayout1->addWidget(lePaperHeight, 5, 2);
	gridlayout1->addWidget(cbPaperUnits, 5, 3);

	gbSize->setLayout(gridlayout1);
	gbSize->setEnabled(false);

	// ------------------------------------------------------------------------
	// Image color controls
	QGridLayout *gridlayout3 = new QGridLayout;
	gridlayout3->addWidget(new QLabel("White"),0,0);
	gridlayout3->addWidget(slWhite,0,1);
	gridlayout3->addWidget(sbWhite,0,2);
	gridlayout3->addWidget(new QLabel("Black"),1,0);
	gridlayout3->addWidget(slBlack,1,1);
	gridlayout3->addWidget(sbBlack,1,2);

	gbAdjust->setLayout(gridlayout3);

	// ------------------------------------------------------------------------
	// Left panel widget
	QVBoxLayout *vboxlayout2 = new QVBoxLayout;
	vboxlayout2->addWidget(fileBrowser);
	vboxlayout2->addWidget(gbSize);
	vboxlayout2->addWidget(gbAdjust);
	vboxlayout2->addWidget(pbSave);
	QWidget *leftPanelWidget = new QWidget;
	leftPanelWidget->setLayout(vboxlayout2);

	// ------------------------------------------------------------------------
	// Both page viewers
	QHBoxLayout *viewerLayout = new QHBoxLayout;
	viewerLayout->addWidget(viewerOriginal);
	viewerLayout->addWidget(viewerPage);
	QWidget *rightPanelWidget = new QWidget;
	rightPanelWidget->setLayout(viewerLayout);

	// ------------------------------------------------------------------------
	// Join them with a splitter
	QSplitter *splitter = new QSplitter(this);
	splitter->addWidget(leftPanelWidget);
	splitter->addWidget(rightPanelWidget);
	setCentralWidget(splitter);

	// Allow more space for the images
	double sr = 0.20;
	int w1 = width()*sr;
	int w2 = width()*(1-sr);
	splitter->setSizes(QList<int>({w1,w2}));
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

QString MainWindow::readRootFolder() {
	QSettings settings("./settings.ini", QSettings::IniFormat);
	settings.beginGroup("FileBrowser");
	QString rootFolder = settings.value("rootFolder", QDir::home().path()).toString();
	settings.endGroup();

	return rootFolder;
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

void MainWindow::scale() {
	if (imgPageOriginal.isNull() || fileName.isEmpty()) {
		return;
	}

	int w = lePixelWidth->text().toInt();
	int h = lePixelHeight->text().toInt();

	imgPageScalled = imgPageOriginal.scaled(w, h, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
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

void MainWindow::selectFile(const QModelIndex &index) {
	fileName = fileModel->filePath(index);
	bool fromScanned = fileName.contains("_scanned");

	if (fromScanned || fileModel->isDir(index)) {
		//original->setPixmap(QPixmap());
		//page->setPixmap(QPixmap());
		viewerOriginal->setPixmap(QPixmap());
		viewerPage->setPixmap(QPixmap());
		imgPageOriginal = QImage();
		fileName = QString();

		gbSize->setEnabled(false);
		gbAdjust->setEnabled(false);

	} else {
		// The the opened image
		QImageReader reader(fileName);
		reader.setAutoTransform(true);
		const QImage inImage = reader.read();

		//original->setPixmap(QPixmap::fromImage(inImage));
		viewerOriginal->setPixmap(QPixmap::fromImage(inImage));

		lbInputImageSize->setText(QString("%1 x %2 px").arg(inImage.width()).arg(inImage.height()));

		// Scan it
		imgPageOriginal = scan(inImage);

		updatePageSizeTemplate();

		gbSize->setEnabled(true);
		gbAdjust->setEnabled(true);
	}
}

void MainWindow::setRootFolder() {
	QString rootFolder = QFileDialog::getExistingDirectory(this, "Select root folder", QDir::home().path());
	if (rootFolder.length() > 0) {
		fileModel->setRootPath(rootFolder);
		fileBrowser->setRootIndex(fileModel->index(rootFolder));
		writeRootFolder(rootFolder);
	}
}

void MainWindow::updateDpi() {
	if (cbPageSizeTemplate->currentIndex() == 0) {
		updatePixelSize();
	} else if (cbPageSizeTemplate->currentIndex() == 1) {
		if (rbPixelSizeMode->isChecked()) {
			updatePixelSize();
		} else {
			updatePaperSize();
		}
	} else if (cbPageSizeTemplate->currentIndex() > 1) {
		updatePaperSize();
	}
}

void MainWindow::updatePageSizeTemplate() {
	updatePixelPaperMode();

	if (cbPageSizeTemplate->currentIndex() == 1) {
		rbAspectRatioLock->setEnabled(true);
		rbAspectRatioFree->setEnabled(true);
		rbPixelSizeMode->setEnabled(true);
		rbPaperSizeMode->setEnabled(true);
		cbPaperUnits->setEnabled(true);
	} else {
		rbAspectRatioLock->setEnabled(false);
		rbAspectRatioFree->setEnabled(false);
		rbPixelSizeMode->setEnabled(false);
		rbPaperSizeMode->setEnabled(false);
		cbPaperUnits->setEnabled(false);
	}

	switch (cbPageSizeTemplate->currentIndex()) {
		case 0:
			lePixelWidth->setText(QString::number(imgPageOriginal.width()));
			lePixelHeight->setText(QString::number(imgPageOriginal.height()));
			updatePixelSize();
			break;

		case 1:
			QMetaObject::invokeMethod(lePixelWidth,"textEdited", Qt::DirectConnection, Q_ARG(QString, ""));
			break;

		case 2:
			lePaperWidth->setText("8.5");
			lePaperHeight->setText("11.0");
			cbPaperUnits->setCurrentIndex(2);
			updatePaperSize();
			break;

		case 3:
			lePaperWidth->setText("8.5");
			lePaperHeight->setText("14.0");
			cbPaperUnits->setCurrentIndex(2);
			updatePaperSize();
			break;

		case 4:
			lePaperWidth->setText("210");
			lePaperHeight->setText("297");
			cbPaperUnits->setCurrentIndex(0);
			updatePaperSize();
			break;
	}
}

void MainWindow::updatePaperSize() {
	double dpi = leDpi->text().toDouble();
	double pw = lePaperWidth->text().toDouble();
	double ph = lePaperHeight->text().toDouble();
	double units = 1;

	if(cbPaperUnits->currentIndex() == 0) {
		units = 25.4;
	} else if(cbPaperUnits->currentIndex() == 1) {
		units = 2.5;
	}

	if(rbAspectRatioLock->isChecked()) {
		double ar = (double) imgPageOriginal.width() / (double) imgPageOriginal.height();
		QLineEdit *obj = qobject_cast<QLineEdit*>(sender());
		if (obj == lePaperWidth) {
			ph = pw / ar;
			lePaperHeight->setText(QString::number(ph, 'f', 1));
		} else if(obj == lePaperHeight) {
			pw = ph * ar;
			lePaperWidth->setText(QString::number(pw, 'f', 1));
		}
	}

	int w = pw * dpi / units;
	int h = ph * dpi / units;

	lePixelWidth->setText(QString::number(w));
	lePixelHeight->setText(QString::number(h));

	scale();
	adjust();
}

void MainWindow::updatePixelPaperMode() {
	if (cbPageSizeTemplate->currentIndex() == 1) {
		if (rbPixelSizeMode->isChecked()) {
			lePixelWidth->setEnabled(true);
			lePixelHeight->setEnabled(true);
			lePaperWidth->setEnabled(false);
			lePaperHeight->setEnabled(false);
		} else {
			lePixelWidth->setEnabled(false);
			lePixelHeight->setEnabled(false);
			lePaperWidth->setEnabled(true);
			lePaperHeight->setEnabled(true);
		}
	} else {
		lePixelWidth->setEnabled(false);
		lePixelHeight->setEnabled(false);
		lePaperWidth->setEnabled(false);
		lePaperHeight->setEnabled(false);
	}
}

void MainWindow::updatePixelSize() {

	double dpi = leDpi->text().toDouble();
	int pw = lePixelWidth->text().toInt();
	int ph = lePixelHeight->text().toInt();

	if(rbAspectRatioLock->isChecked()) {
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

	lePaperWidth->setText(QString::number(w, 'f', 1));
	lePaperHeight->setText(QString::number(h, 'f', 1));

	scale();
	adjust();
}

void MainWindow::writeRootFolder(QString rootFolder) {
	QSettings settings("./settings.ini", QSettings::IniFormat);
	settings.beginGroup("FileBrowser");
	settings.setValue("rootFolder", rootFolder);
	settings.endGroup();
}


















