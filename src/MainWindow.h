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

#include "LayerGraphicWidget.h"
#include "PixelLayer.h"
#include "RoiEditorLayer.h"

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
		void createObjects();
		void createLayout();

		// Settings
		QString readRootFolder();
		void writeRootFolder(QString rootFolder);

		// Logic
		void adjust();
		void save();
		void scale();
		void findPage();
		void scanPage();

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
		QImage imgOriginal;
		QImage imgPageOriginal;
		QImage imgPageScaled;
		QImage imgPageProcessed;

		// Image viewer
        LayerGraphicWidget *viewerOriginal;
        PixelLayer *imageLayerOriginal;
        RoiEditorLayer *roiEditorLayerOriginal;

        LayerGraphicWidget *viewerPage;
        PixelLayer *imageLayerPage;
};



#endif /* SRC_MAINWINDOW_H_ */
