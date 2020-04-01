/*
 * MainWindow.h
 *
 *  Created on: 22 sep. 2019
 *      Author: claud
 */

#ifndef SRC_MAINWINDOW_H_
#define SRC_MAINWINDOW_H_

#include <QtWidgets/QtWidgets>
#include <opencv4/opencv2/opencv.hpp>

#include "PageViewer.h"
#include "PageMapper.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);

	private slots:
		void about();
		void adjustImage();
		void selectFile(const QModelIndex &index);
		void setRootFolder();
		void updateDpi();
		void updatePageSizeTemplate();
		void updatePaperSize();
		void updatePixelPaperMode();
		void updatePixelSize();
		void updateScanPage();


	private:
		// GUI
		void createActions();
		void createConnections();
		void createControlObject();
		void createLayout();

		// Settings
		QString readRootFolder();
		void writeRootFolder(QString rootFolder);

		// Logic
		void adjust();
		QImage cvMatToQImage(const cv::Mat &inMat);
		cv::Mat QImageToCvMat(const QImage &inImage);
		void save();
		void scale();
		void findPage(const QImage &inImage);
		QImage scanPage(const QImage &inImage);


	private:
		// File browser
		QString fileName;
		QFileSystemModel *fileModel;
		QTreeView *fileBrowser;

		// Image size settings
		QGroupBox *gbSize;
		QLabel *lbInputImageSize;
		QRadioButton *rbAspectRatioLock;
		QRadioButton *rbAspectRatioFree;
		QRadioButton *rbPixelSizeMode;
		QRadioButton *rbPaperSizeMode;
		QComboBox *cbPageSizeTemplate;
		QLineEdit *leDpi;
		QLineEdit *lePixelWidth;
		QLineEdit *lePixelHeight;
		QLineEdit *lePaperWidth;
		QLineEdit *lePaperHeight;
		QComboBox *cbPaperUnits;

		// Image color adjustment settings
		QGroupBox *gbAdjust;
		QSlider *slWhite;
		QSlider *slBlack;
		QSpinBox *sbWhite;
		QSpinBox *sbBlack;
		QPushButton *pbSave;

		// Page viewers
		PageViewer *viewerOriginal;
		PageViewer *viewerPage;
		PageMapper *corners;
		QImage imgOriginal;
		QImage imgPageOriginal;
		QImage imgPageScalled;
		QImage imgPageProcessed;
};



#endif /* SRC_MAINWINDOW_H_ */
