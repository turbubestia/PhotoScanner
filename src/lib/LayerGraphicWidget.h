/*
 * LayeredImageEditor.h
 *
 *  Created on: Jun 8, 2020
 *      Author: claud
 */

#ifndef SRC_LIBS_LAYERGRAPHICWIDGET_H_
#define SRC_LIBS_LAYERGRAPHICWIDGET_H_

#include <QtWidgets/QtWidgets>

class AbstractLayer;

class LayerGraphicWidget : public QWidget  {
    Q_OBJECT

    public:
        LayerGraphicWidget(QWidget *parent = nullptr);
        virtual ~LayerGraphicWidget();

        void addLayer(AbstractLayer *layer);
        QSize canvasSize();
        QRect getRoi() { return roi; }
        void resizeCanvas(int w, int h);
        void resizeCanvasToLayer(int layer);
        void setCoreInteration(bool enabled);
        void setZoom(double zoom);
        double getScale();
        QPoint mapWidgetToCanvas(QPoint pos);
		QPoint mapCanvasToWidget(QPoint pos);

    private:
        void updateRoi();

    protected:
        void keyPressEvent(QKeyEvent *event) override;
        void keyReleaseEvent(QKeyEvent *event) override;
        void mouseDoubleClickEvent(QMouseEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void paintEvent(QPaintEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;

    private:
        int _canvasWidth;
        int _canvasHeight;
        QImage _widgetBackground;

        double zoom;
        double scale;

        bool alwaysCoreInteration;
        bool coreInteration;
        bool fit;
        bool drag;

        QRect roi;
        QPoint roiPivot;
        QPoint lastDragPoint;

        QList<AbstractLayer *> layers;
        int activeLayer;
};

#endif /* SRC_LIBS_LAYERGRAPHICWIDGET_H_ */
