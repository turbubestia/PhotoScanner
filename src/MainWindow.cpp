/*
 * MainWindow.cpp
 *
 *  Created on: 22 sep. 2019
 *      Author: claud
 */

#include <QtCore/QDebug>

#include <QtWidgets/QtWidgets>
#include <QtGui/QFileSystemModel>

#include <opencv4/opencv2/opencv.hpp>
#include <opencv4/opencv2/core/saturate.hpp>

#include "MainWindow.h"
#include "Utils.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	// Resize to a reasonable size
	resize(QGuiApplication::primaryScreen()->availableSize() * 4/5);

	createActions();
	createObjects();
	createLayout();
	createConnections();

	sbWhite->setValue(235);
	sbBlack->setValue(40);
}

void MainWindow::about() {

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

    connect(pageSizeControl, &ImageSizeControl::sizeChanged, this, &MainWindow::imageSizeChanged);

	connect(slWhite, &QSlider::valueChanged, sbWhite, &QSpinBox::setValue);
    connect(sbWhite, QOverload<int>::of(&QSpinBox::valueChanged), slWhite, &QSlider::setValue);
	connect(slBlack, &QSlider::valueChanged, sbBlack, &QSpinBox::setValue);
    connect(sbBlack, QOverload<int>::of(&QSpinBox::valueChanged), slBlack, &QSlider::setValue);

	connect(pbSave, &QPushButton::clicked, this, &MainWindow::save);
}

void MainWindow::createObjects() {

	// ------------------------------------------------------------------------
	// File browser
	QString rootFolder = readRootFolder();

	fileModel = new QFileSystemModel;
	fileModel->setRootPath(rootFolder);
	fileModel->setNameFilters({"*.jpg", "*.jpeg", "*.png"});
	fileModel->setNameFilterDisables(false);

	fileBrowser = new QTreeView();
	fileBrowser->setModel(fileModel);
	fileBrowser->setRootIndex(fileModel->index(rootFolder));
	fileBrowser->setColumnHidden(1, true);
	fileBrowser->setColumnHidden(2, true);
	fileBrowser->setColumnWidth(0, width()*0.2*0.8);

	// ------------------------------------------------------------------------
	// Both original and process page image
	viewerOriginal = new LayerGraphicWidget;
	imageLayerOriginal = new PixelLayer;
	roiEditorLayerOriginal = new RoiEditorLayer;

	viewerOriginal->addLayer(imageLayerOriginal);
	viewerOriginal->addLayer(roiEditorLayerOriginal);
    viewerOriginal->setCoreInteration(true);

	roiEditorLayerOriginal->setVisible(false);

	viewerPage = new LayerGraphicWidget;
	imageLayerPage = new PixelLayer;

	viewerPage->addLayer(imageLayerPage);
    viewerPage->setCoreInteration(true);

	// ------------------------------------------------------------------------
	// Page size controls
	gbSize = new QGroupBox("Page size");
    pageSizeControl = new ImageSizeControl();

	// ------------------------------------------------------------------------
	// Page color adjustment
	gbAdjust = new QGroupBox("Color Control");

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

    // ---------------------------------------------
    // OpenCV backend
    imgproc = new ImageProcessor;
}

void MainWindow::createLayout() {
	// ------------------------------------------------------------------------
	// Page size group box
    QVBoxLayout *vboxlayout1 = new QVBoxLayout;
    vboxlayout1->addWidget(pageSizeControl);
    gbSize->setLayout(vboxlayout1);
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
    gbAdjust->setEnabled(false);

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

QString MainWindow::readRootFolder() {
	QSettings settings("./settings.ini", QSettings::IniFormat);
	settings.beginGroup("FileBrowser");
	QString rootFolder = settings.value("rootFolder", QDir::home().path()).toString();
	settings.endGroup();

	return rootFolder;
}

void MainWindow::save() {

}

#include <exiv2/exiv2.hpp>

void MainWindow::selectFile(const QModelIndex &index) {
	qDebug() << "MainWindow::selectFile()";

    bool isValid = false;

	fileName = fileModel->filePath(index);

    int imgWidth = 0;
    int imgHeight = 0;
    int dpi = 0;

    if (!fileModel->isDir(index))
    {
        Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(fileName.toStdString());

        image->readMetadata();
        Exiv2::ExifData &exifData = image->exifData();

        if (exifData.count() > 0)
        {
            Exiv2::ExifData::const_iterator end = exifData.end();
            for (Exiv2::ExifData::const_iterator i = exifData.begin(); i != end; ++i) {
                QString key = QString::fromStdString(i->key());

                if(key == "Exif.Image.XResolution") {
                    dpi = i->value().toLong();
                } else if(key == "Exif.Photo.PixelXDimension") {
                    imgWidth = i->value().toLong();
                } else if(key == "Exif.Photo.PixelYDimension") {
                    imgHeight = i->value().toLong();
                }
            }
        }
        else {
            QImageReader imageReader(fileName);
            imageReader.setAutoTransform(true);
            QSize imageSize = imageReader.size();
            imgWidth = imageSize.width();
            imgHeight = imageSize.height();
            dpi = 72;
        }

        pageSizeControl->setSourceSize(imgWidth, imgHeight, dpi);

        if (!fileName.contains("_scanned")) {
            isValid = true;
        }
    }

    if (isValid){
        imgproc->setSourceImage(fileName);

        imageLayerOriginal->setImage(imgproc->getSourceImage());
        imageLayerOriginal->setVisible(true);

        roiEditorLayerOriginal->setRoi(imgproc->findPage());
        roiEditorLayerOriginal->setVisible(true);

        viewerOriginal->resizeCanvasToLayer(0);
        viewerOriginal->update();

        pbSave->setEnabled(true);
        gbSize->setEnabled(true);
        gbAdjust->setEnabled(true);
    }
    else {
        imageLayerOriginal->setVisible(false);
        roiEditorLayerOriginal->setVisible(false);

        pbSave->setEnabled(false);
        gbSize->setEnabled(false);
        gbAdjust->setEnabled(false);
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

void MainWindow::imageSizeChanged(int width, int height)
{

}

void MainWindow::writeRootFolder(QString rootFolder) {
	QSettings settings("./settings.ini", QSettings::IniFormat);
	settings.beginGroup("FileBrowser");
	settings.setValue("rootFolder", rootFolder);
	settings.endGroup();
}


















