/*
 * PixelLayer.cpp
 *
 *  Created on: Jun 10, 2020
 *      Author: claud
 */

#include <QtWidgets/QtWidgets>

#include "LayerGraphicWidget.h"
#include "AbstractLayer.h"
#include "PixelLayer.h"

PixelLayer::PixelLayer() {
}

PixelLayer::~PixelLayer() {

}

const QImage &PixelLayer::image() {
    return _image;
}

void PixelLayer::clear() {
	_image = QImage();
	_width = 100;
	_height = 100;
}

void PixelLayer::render(QPainter *painter) {
    if (!_image.isNull()) {
        painter->drawImage(0,0,_image);
    }
}

void PixelLayer::resize(QSize size) {
    if (!_image.isNull() && _image.size() != size) {
        _image = _image.scaled(size);
        _width = size.width();
        _height = size.height();
    }
}

void PixelLayer::resizeToCanvas() {
    if (!_image.isNull()) {
        _image = _image.scaled(parent()->canvasSize());
    }
}

void PixelLayer::setImage(const QImage &image) {
    QSize size = image.size();
    _image = image;
    if (!image.isNull()) {
    	_width = size.width();
    	_height = size.height();
    } else {
    	_width = _height = 100;
    }
}
