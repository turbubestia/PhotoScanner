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
		QImage scan(const QImage &photo);


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
		QImage imgPageOriginal;
		QImage imgPageScalled;
		QImage imgPageProcessed;
};



#endif /* SRC_MAINWINDOW_H_ */
