/*
 * AbstractOverlay.cpp
 *
 *  Created on: Jun 8, 2020
 *      Author: claud
 */

#include <AbstractLayer.h>
#include <LayerGraphicWidget.h>
#include <QtWidgets/QtWidgets>


AbstractLayer::AbstractLayer() {
    _parent = nullptr;
    _width = 0;
    _height = 0;
    visible = true;
}

AbstractLayer::~AbstractLayer() {

}

bool AbstractLayer::mouseDoubleClickEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    return false;
}

bool AbstractLayer::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    return false;
}

bool AbstractLayer::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    return false;
}

bool AbstractLayer::mouseMoveEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    return false;
}

void AbstractLayer::resize(QSize size) {
	_width = size.width();
	_height = size.height();
}

void AbstractLayer::resizeToCanvas() {
    QSize cs = _parent->canvasSize();
    _width = cs.width();
    _height = cs.height();
}
