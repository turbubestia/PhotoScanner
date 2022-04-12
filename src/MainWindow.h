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
#include "ImageSizeControl.h"
#include "ImageProcessor.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	public:
		MainWindow(QWidget *parent = nullptr);

	private slots:
		void about();
		void selectFile(const QModelIndex &index);
		void setRootFolder();
        void imageSizeChanged(int width, int height);

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
		void save();

	private:
		// File browser
		QString fileName;
		QFileSystemModel *fileModel;
		QTreeView *fileBrowser;

		// Image size settings
        QGroupBox *gbSize;
        ImageSizeControl *pageSizeControl;

		// Image color adjustment settings
		QGroupBox *gbAdjust;
		QSlider *slWhite;
		QSlider *slBlack;
		QSpinBox *sbWhite;
		QSpinBox *sbBlack;
		QPushButton *pbSave;

		// Image viewer
        LayerGraphicWidget *viewerOriginal;
        PixelLayer *imageLayerOriginal;
        RoiEditorLayer *roiEditorLayerOriginal;

        LayerGraphicWidget *viewerPage;
        PixelLayer *imageLayerPage;

        // opencv backend
        ImageProcessor *imgproc;
};



#endif /* SRC_MAINWINDOW_H_ */
