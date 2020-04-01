/*
 * PageViewer.cpp
 *
 *  Created on: 13 oct. 2019
 *      Author: claud
 */

#include "PageViewer.h"
#include "PageMapper.h"

PageViewer::PageViewer(QWidget *parent)
	: QWidget(parent) {
	dpi = 150;
	ratio = 1.0;
	zoom = 1.0;
	corners = nullptr;
	moveCornerIndex = -1;
}

PageViewer::~PageViewer() {

}

void PageViewer::setMapper(PageMapper *corners) {
	this->corners = corners;
	update();
}

void PageViewer::setPixmap(const QPixmap &pixmap) {
	page = pixmap;
	update();
}

QSize PageViewer::sizeHint() const {
	return page.size();
}

const QPixmap & PageViewer::getPixmap() {
	return page;
}

QPoint PageViewer::mapWidgetToImage(QPoint pos) {
	// widget size
	int w = width();
	int h = height();
	// image size
	int iw = page.width()*ratio;
	int ih = page.height()*ratio;
	// offset
	int ow = (w - iw)/2;
	int oh = (h - ih)/2;
	// image relative position
	int px = (pos.x() - ow)/ratio;
	int py = (pos.y() - oh)/ratio;

	return QPoint(px,py);
}

QPoint PageViewer::mapImageToWidget(QPoint pos) {
	// widget size
	int w = width();
	int h = height();
	// image size
	int iw = page.width()*ratio;
	int ih = page.height()*ratio;
	// offset
	int ow = (w - iw)/2;
	int oh = (h - ih)/2;
	// image to widget scale transformation
	int ix = pos.x()*ratio;
	int iy = pos.y()*ratio;

	return QPoint(ow+ix, oh+iy);
}

void PageViewer::mouseMoveEvent(QMouseEvent *event) {
	QPoint pos = mapWidgetToImage(event->pos());
	double dx = (lastMousePosUn.x() - pos.x()) * ratio / 2;
	double dy = (lastMousePosUn.y() - pos.y()) * ratio / 2;
	QPointF newPos = lastMousePos - QPointF(dx, dy);

	corners->setCornerPosition(moveCornerIndex, newPos.toPoint());
	event->accept();

	lastMousePos = newPos;
	lastMousePosUn = pos;
	update();
}

void PageViewer::mousePressEvent(QMouseEvent *event) {
	if (corners != nullptr) {
		lastMousePos = mapWidgetToImage(event->pos());
		lastMousePosUn = lastMousePos;
		moveCornerIndex = corners->indexOf(lastMousePos.toPoint(), ratio);
	}
	event->accept();
}

void PageViewer::mouseReleaseEvent(QMouseEvent *event) {
	moveCornerIndex = -1;
	event->accept();
}

void PageViewer::paintEvent(QPaintEvent *) {
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	int w = width();
	int h = height();

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

	if (page.isNull()) {
		return;
	}

	// Original page size
	int iw = page.width();
	int ih = page.height();

	// Make the image size maximum 90% of width or height
	double rw = 0.95*h/ih;
	if (rw > 1.0) {
		rw = 1.0;
	}
	double rh = 0.95*w/iw;
	if (rh > 1.0) {
		rh = 1.0;
	}

	ratio = rh > rw ? rw : rh;
	iw = iw*ratio;
	ih = ih*ratio;
	QPixmap temp = page.scaled(iw, ih, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	painter.drawPixmap(w/2-iw/2, h/2-ih/2, temp);

	// paint the page rectangle
	if (corners != nullptr) {

		QImage mask(w,h,QImage::Format_ARGB32);
		mask.fill(QColor(150,20,20,127));

		QPainter maskPainter(&mask);

		maskPainter.setRenderHint(QPainter::Antialiasing);
		maskPainter.setPen(Qt::red);
		maskPainter.setBrush(QBrush(QColor(0,0,0,0)));
		QPoint *p = new QPoint[4];
		for (int i = 0; i < 4; i++) {
			p[i] = mapImageToWidget(corners->getCornerPosition(i));
		}
		maskPainter.setCompositionMode(QPainter::CompositionMode_Source);
		maskPainter.drawPolygon(p,4);

		painter.drawImage(0,0,mask);
	}
}
