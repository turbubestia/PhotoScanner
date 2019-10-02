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

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);

	private slots:
		void clicked(const QModelIndex &index);

		void SizeSourceChanged();
		void pageSizeSelected();
		void pagePixelSizeChanged();
		void pagePaperSizedChanged();
		void pageDpiChanged();

		void sizeChanged();
		void controlChanged();

	private:
		cv::Mat QImageToCvMat(const QImage &inImage);
		QImage cvMatToQImage(const cv::Mat &inMat);
		QImage scan(const QImage &photo);
		void scale();
		void adjust();
		void save();

	private:
		QLabel *original;
		QLabel *page;
		QString fileName;

		QImage imgPageOriginal;
		QImage imgPageScalled;
		QImage imgPageProcessed;

		// File brower
		QFileSystemModel *fileModel;
		QTreeView *fileBrowser;

		// Image size settings
		QGroupBox *gbSize;

		QLabel *lbInputImageSize;

		QRadioButton *rbAspectRatioLocked;
		QRadioButton *rbAspectRatioFree;
		QRadioButton *rbPixelLock;
		QRadioButton *rbPaperLock;

		QComboBox *cbPageSize;
		QLineEdit *leDpi;

		QLineEdit *lePixelWidth;
		QLineEdit *lePixelHeight;
		QLineEdit *lePhysicalWidth;
		QLineEdit *lePhysicalHeight;
		QComboBox *cbUnits;

		QGroupBox *gbAdjust;
		QSlider *slWhite;
		QSlider *slBlack;
		QSpinBox *sbWhite;
		QSpinBox *sbBlack;
		QPushButton *pbSave;
};



#endif /* SRC_MAINWINDOW_H_ */
