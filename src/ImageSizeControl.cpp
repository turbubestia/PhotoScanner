#include "ImageSizeControl.h"
#include "ui_ImageSizeControl.h"

ImageSizeControl::ImageSizeControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageSizeControl)
{
    ui->setupUi(this);
    ui->pageTemplate->setCurrentIndex(0);

    on_pageTemplate_currentIndexChanged(0);
    setSourceSize(300*8.5,300*11,300);
}

ImageSizeControl::~ImageSizeControl()
{
    delete ui;
}

void ImageSizeControl::setSourceSize(int pixelWidth, int pixelHeight, double dpi)
{
    sourcePixelWidth = pixelWidth;
    sourcePixelHeight = pixelHeight;
    sourceDpi = dpi;
    double sourcePageWidth = pixelWidth / dpi * getUnitMultiplier();
    double sourcePageHeight = pixelHeight / dpi * getUnitMultiplier();

    ui->sourceImageSize->setText(QString("<b>%1</b> x <b>%2</b> px (%3 dpi)").arg(pixelWidth).arg(pixelHeight).arg(sourceDpi));

    double dpu = ui->dpi->value() / getUnitMultiplier();

    if (ui->pageTemplate->currentText() == "Source") {
        ui->pixelWidth->setValue(sourcePageWidth * dpu);
        ui->pixelHeight->setValue(sourcePageHeight * dpu);
        ui->paperWidth->setValue(sourcePageWidth);
        ui->paperHeight->setValue(sourcePageHeight);
    } else {
        double pxw = ui->paperWidth->value() * dpu;
        double pxh = ui->paperHeight->value() * dpu;
        ui->pixelWidth->setValue(pxw);
        ui->pixelHeight->setValue(pxh);
    }
}

double ImageSizeControl::getAspectRatioFromTemplate()
{
    double aspectRatio;

    if (ui->pageTemplate->currentText() == "Source") {
        aspectRatio = (double) sourcePixelWidth / (double) sourcePixelHeight;
    } else if (ui->pageTemplate->currentText() == "Custom") {
        aspectRatio = -1;
    } else if (ui->pageTemplate->currentText() == "Letter") {
        aspectRatio = 85.0/110.0;
    } else if (ui->pageTemplate->currentText() == "Legal") {
        aspectRatio = 85.0/140.0;
    } else if (ui->pageTemplate->currentText() == "A4") {
        aspectRatio = 210.0/297.0;
    } else {
        aspectRatio = -1;
    }

    return aspectRatio;
}

double ImageSizeControl::getUnitMultiplier()
{
    if (ui->paperUnits->currentText() == "mm") {
        return 25.4;
    } else if(ui->paperUnits->currentText() == "cm") {
        return 2.54;
    } else {
        return 1.0;
    }
}

void ImageSizeControl::on_pageTemplate_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    // The paper size can only be changed on custom template
    ui->paperWidth->setEnabled(index == 1);
    ui->paperHeight->setEnabled(index == 1);

    // All templates, except the custom one, change the paper size, and
    // therefore, they lock the aspect ratio
    if (index == 0) {
        ui->paperWidth->setValue(sourcePixelWidth / sourceDpi * getUnitMultiplier());
        ui->paperHeight->setValue(sourcePixelHeight / sourceDpi * getUnitMultiplier());
    }
    else if (index >= 2) {
        int idx = index - 2;
        int units[] = {2, 2, 0};
        double templateWidth[] = {8.5, 8.5, 210};
        double templateHeight[] = {11, 14, 297};

        ui->paperUnits->setCurrentIndex(units[idx]);
        ui->paperWidth->setValue(templateWidth[idx]);
        ui->paperHeight->setValue(templateHeight[idx]);
    }

    // Adjust the pixel size based on the aspect ratio and dpi
    double aspectRatio = getAspectRatioFromTemplate();

    if (aspectRatio != -1) {
        double dpu = ui->dpi->value() / getUnitMultiplier();
        double pgw = ui->paperWidth->value();
        double pgh = ui->paperHeight->value();

        ui->pixelWidth->setValue(pgw * dpu);
        ui->pixelHeight->setValue(pgh * dpu);
    }

    emit sizeChanged(ui->pixelWidth->value(), ui->pixelHeight->value());
}


void ImageSizeControl::on_pixelWidth_editingFinished()
{
    double aspectRatio = getAspectRatioFromTemplate();
    double pxw = ui->pixelWidth->value();

    if (aspectRatio != -1)
        ui->pixelHeight->setValue(round(pxw / aspectRatio));

    ui->dpi->setValue(pxw / ui->paperWidth->value() * getUnitMultiplier());

    emit sizeChanged(ui->pixelWidth->value(), ui->pixelHeight->value());
}


void ImageSizeControl::on_pixelHeight_editingFinished()
{
    double aspectRatio = getAspectRatioFromTemplate();
    double pxh = ui->pixelHeight->value();

    if (aspectRatio != -1)
        ui->pixelWidth->setValue(round(pxh * aspectRatio));

    ui->dpi->setValue(pxh / ui->paperHeight->value() * getUnitMultiplier());

    emit sizeChanged(ui->pixelWidth->value(), ui->pixelHeight->value());
}


void ImageSizeControl::on_paperWidth_editingFinished()
{
    double dpu = ui->dpi->value() / getUnitMultiplier();
    double pgw = ui->paperWidth->value();

    ui->pixelWidth->setValue(pgw * dpu);

    emit sizeChanged(ui->pixelWidth->value(), ui->pixelHeight->value());
}


void ImageSizeControl::on_paperHeight_editingFinished()
{
    double dpu = ui->dpi->value() / getUnitMultiplier();
    double pgh = ui->paperHeight->value();

    ui->pixelHeight->setValue(pgh * dpu);

    emit sizeChanged(ui->pixelWidth->value(), ui->pixelHeight->value());
}


void ImageSizeControl::on_paperUnits_currentIndexChanged(int index)
{
    Q_UNUSED(index);

    double pxw = ui->pixelHeight->value();
    double pxh = ui->pixelWidth->value();
    double dpu = ui->dpi->value() / getUnitMultiplier();

    ui->paperWidth->setValue(pxw / dpu);
    ui->paperHeight->setValue(pxh / dpu);
}


void ImageSizeControl::on_dpi_editingFinished()
{
    double pgw = ui->paperWidth->value();
    double pgh = ui->paperHeight->value();
    double dpu = ui->dpi->value() / getUnitMultiplier();

    ui->pixelWidth->setValue(pgw * dpu);
    ui->pixelHeight->setValue(pgh * dpu);

    emit sizeChanged(ui->pixelWidth->value(), ui->pixelHeight->value());
}

