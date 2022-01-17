/*
 * LayeredImageEditor.cpp
 *
 *  Created on: Jun 8, 2020
 *      Author: claud
 */

#include "LayerGraphicWidget.h"

#include "AbstractLayer.h"

LayerGraphicWidget::LayerGraphicWidget(QWidget *parent)
    : QWidget(parent) {

    zoom = 1;
    scale = 1;

    alwaysCoreInteration = false;
    coreInteration = alwaysCoreInteration;
    fit = true;
    drag = false;

    QImageReader reader("canvas.jpg");
    reader.setAutoTransform(true);
    _canvasWidth = 100;
    _canvasHeight = 100;

    activeLayer = -1;

    roi.setRect(0,0,_canvasWidth,_canvasHeight);

    updateRoi();

    setFocusPolicy(Qt::ClickFocus);
}

LayerGraphicWidget::~LayerGraphicWidget() {

}

void LayerGraphicWidget::addLayer(AbstractLayer *layer) {
    layer->setParent(this);
    layers.append(layer);
    activeLayer = layers.count()-1;
    coreInteration = alwaysCoreInteration;
}

QSize LayerGraphicWidget::canvasSize() {
    return QSize(_canvasWidth, _canvasHeight);
}

double LayerGraphicWidget::getScale() {
	return scale;
}

void LayerGraphicWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        coreInteration = true;
        event->accept();
    } else {

    }
}

void LayerGraphicWidget::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Space) {
        coreInteration = alwaysCoreInteration;
        event->accept();
    } else {

    }
}

QPoint LayerGraphicWidget::mapWidgetToCanvas(QPoint pos) {
    int cx = roi.x() + (pos.x()-10) / scale;
    int cy = roi.y() + (pos.y()-10) / scale;

    return QPoint(cx,cy);
}

QPoint LayerGraphicWidget::mapCanvasToWidget(QPoint pos) {
    int wx = (pos.x() - roi.x()) * scale + 10;
    int wy = (pos.y() - roi.y()) * scale + 10;

    return QPoint(wx,wy);
}

void LayerGraphicWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    bool ruleCoreInteration = coreInteration;

    if (!ruleCoreInteration) {
        if (layers.count() > 0 && layers.at(activeLayer)->isVisible()) {
            ruleCoreInteration = (layers.at(activeLayer)->mouseDoubleClickEvent(event) == false);
        }
    }

    if (ruleCoreInteration) {
        // Image pivot point
        if (fit) {
            fit = false;
            roiPivot = mapWidgetToCanvas(event->pos());
        } else {
            fit = true;
        }

        updateRoi();
    }

    update();

    event->accept();
}

void LayerGraphicWidget::mousePressEvent(QMouseEvent *event) {
    bool ruleCoreInteration = coreInteration;

    if (!ruleCoreInteration) {
        if (layers.count() > 0 && layers.at(activeLayer)->isVisible()) {
            ruleCoreInteration = (layers.at(activeLayer)->mousePressEvent(event) == false);
        }
    }

    if (ruleCoreInteration) {
        drag = true;
        lastDragPoint = mapWidgetToCanvas(event->pos());
    }

    event->accept();
}

void LayerGraphicWidget::mouseReleaseEvent(QMouseEvent *event) {
    bool ruleCoreInteration = coreInteration;

    if (!ruleCoreInteration) {
        if (layers.count() > 0 && layers.at(activeLayer)->isVisible()) {
            ruleCoreInteration = (layers.at(activeLayer)->mouseReleaseEvent(event) == false);
        }
    }

    if (ruleCoreInteration) {
        drag = false;
    }

    event->accept();
}

void LayerGraphicWidget::mouseMoveEvent(QMouseEvent *event) {
    bool ruleCoreInteration = coreInteration;

    if (!ruleCoreInteration) {
        if (layers.count() > 0 && layers.at(activeLayer)->isVisible()) {
            ruleCoreInteration = (layers.at(activeLayer)->mouseMoveEvent(event) == false);
        }
    }

    if (ruleCoreInteration) {
        if (drag) {
            roi.translate(lastDragPoint - mapWidgetToCanvas(event->pos()));
        }
    }

    update();

    event->accept();
}

void LayerGraphicWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    if (!_widgetBackground.isNull()) {
        painter.drawImage(QPoint(0,0), _widgetBackground);
    }

    // Transformation matrix
    QMatrix view;
    view.translate(10, 10); // Board of 10 pixels in screen space
    view.scale(scale, scale); // Transform to image size space
    view.translate(-roi.x(), -roi.y()); //
    painter.setTransform(QTransform(view));

    // Render each layer in order
    for (int i = 0; i < layers.count(); i++) {
        layers.at(i)->render(&painter);
    }

    event->accept();
}

void LayerGraphicWidget::resizeEvent(QResizeEvent *event) {
    int w = width();
    int h = height();

    // cache the widget background
    _widgetBackground = QImage(w, h, QImage::Format_RGB32);
    QPainter painter(&_widgetBackground);

    // Background grid
    QColor color1 = QColor(225,225,225,255);
    QColor color2 = QColor(200,200,200,255);
    for (int y = 0; y < h; y += 10) {
        QRect square(0,y,10,10);
        for (int x = 0; x < w; x += 10) {
            if (((x+y) / 10) & 1) {
                painter.setBrush(color1);
                painter.setPen(color1);
            } else {
                painter.setBrush(color2);
                painter.setPen(color2);
            }
            painter.drawRect(square);
            square.translate(10,0);
        }
    }

    painter.end();

    updateRoi();
    update();

    event->accept();
}

void LayerGraphicWidget::resizeCanvas(int w, int h) {
    _canvasWidth = w;
    _canvasHeight = h;

    fit = true;
    updateRoi();
    update();
}

void LayerGraphicWidget::resizeCanvasToLayer(int layer) {
    if (layer >= 0 && layer < layers.count()) {
        QSize ls = layers.at(layer)->size();
        _canvasWidth = ls.width();
        _canvasHeight = ls.height();

        fit = true;
        updateRoi();
        update();
    }
}

void LayerGraphicWidget::setCoreInteration(bool enabled) {
    alwaysCoreInteration = enabled;
    coreInteration = alwaysCoreInteration;
}

void LayerGraphicWidget::setZoom(double zoom) {
    this->zoom = zoom;
    if (!fit) {
        updateRoi();
        update();
    }
}

void LayerGraphicWidget::updateRoi() {
    // Widget size
    int w = width()-20;
    int h = height()-20;

    // Original page size
    int iw = _canvasWidth;
    int ih = _canvasHeight;

    // The scale factor is use to compute the size of the RoI
    if (fit) {
        double rw, rh, rx, ry;

        // Zoom required to fit the image to the widget
        rw = (double) h / ih;
        if (rw > 1.0) {
            rw = 1.0;
        }
        rh = (double) w / iw;
        if (rh > 1.0) {
            rh = 1.0;
        }

        // Zoom to the largest dimension
        scale = rh > rw ? rw : rh;

        // Update the region of interest
        rw = w/scale;
        rh = h/scale;
        rx = rw > _canvasWidth ? (_canvasWidth - rw) / 2 : 0;
        ry = rh > _canvasHeight ? (_canvasHeight - rh) / 2 : 0;
        roi.setRect(rx,ry,rw,rh);

    } else {
        scale = zoom;

        // normalize pivot position
        double px = (double) (roiPivot.x() - roi.x()) / roi.width();
        double py = (double) (roiPivot.y() - roi.y()) / roi.height();

        // Update the region of interest
        double rw = w/scale;
        double rh = h/scale;
        double rx = roiPivot.x() - w*px/scale;
        double ry = roiPivot.y() - h*py/scale;
        roi.setRect(rx,ry,rw,rh);
    }
}
