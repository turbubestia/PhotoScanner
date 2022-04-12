#ifndef IMAGESIZECONTROL_H
#define IMAGESIZECONTROL_H

#include <QtCore>
#include <QWidget>

namespace Ui {
class ImageSizeControl;
}

class ImageSizeControl : public QWidget
{
        Q_OBJECT

    public:
        explicit ImageSizeControl(QWidget *parent = nullptr);
        ~ImageSizeControl();

        void setSourceSize(int pixelWidth, int pixelHeight, double dpi);

        double getAspectRatioFromTemplate();
        double getUnitMultiplier();

    signals:
        void sizeChanged(int width, int height);

    private slots:
        void on_pageTemplate_currentIndexChanged(int index);
        void on_pixelWidth_editingFinished();
        void on_pixelHeight_editingFinished();
        void on_paperWidth_editingFinished();
        void on_paperHeight_editingFinished();
        void on_paperUnits_currentIndexChanged(int index);
        void on_dpi_editingFinished();

    private:
        Ui::ImageSizeControl *ui;

        int sourcePixelWidth;
        int sourcePixelHeight;
        double sourceDpi;
};

#endif // IMAGESIZECONTROL_H
