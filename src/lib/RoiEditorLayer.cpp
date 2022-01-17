/*
 * RoiEditor.cpp
 *
 *  Created on: 28 jun. 2020
 *      Author: claud
 */

#include "LayerGraphicWidget.h"
#include "RoiEditorLayer.h"

RoiEditorLayer::RoiEditorLayer() {
    corners.fill(QPoint(0,0), 4);

	drag = false;
	corner = 0;
	r = 7;
}

RoiEditorLayer::~RoiEditorLayer() {

}

QPoint RoiEditorLayer::getCornerPosition(int index) {
    if (index < 0 || index > 3) {
        return QPoint();
    }
    return corners.at(index).toPoint();
}

void RoiEditorLayer::setCornerPosition(int index, QPoint pos) {
    if (index < 0 || index > 3) {
        return;
    }

    corners[index] = pos;
    emit changed();
}

bool RoiEditorLayer::mousePressEvent(QMouseEvent *event) {
	lastDragPoint = parent()->mapWidgetToCanvas(event->pos());

	int cx = lastDragPoint.x();
	int cy = lastDragPoint.y();

	double rr = r/parent()->getScale();

	corner = -1;
	for(int i = 0; i < 4; i++) {
		double dx = cx - corners.at(i).x();
		double dy = cy - corners.at(i).y();
		double d = sqrt(dx*dx+dy*dy);
		if (d < rr) {
			corner = i;
			break;
		}
	}

	drag = (corner != -1);

	return drag;
}

bool RoiEditorLayer::mouseReleaseEvent(QMouseEvent *event) {
	Q_UNUSED(event);

	if (drag) {
		drag = false;
		return true;
	} else {
		return false;
	}
}

bool RoiEditorLayer::mouseMoveEvent(QMouseEvent *event) {
	if (drag) {
		corners[corner] = parent()->mapWidgetToCanvas(event->pos());

		emit changed();

		return true;
	} else {
		return false;
	}
}

void RoiEditorLayer::render(QPainter *painter) {

	if (!isVisible()) {
		return;
	}

	painter->save();

	QPen pen;
	pen.setColor(QColor(250,20,20,255));
	pen.setWidthF(1.5/parent()->getScale());
	painter->setPen(pen);

	painter->setBrush(QColor(0,0,0,0));

	double rr = r/parent()->getScale();

	painter->drawEllipse(corners.at(0),rr,rr);
	painter->drawEllipse(corners.at(1),rr,rr);
	painter->drawEllipse(corners.at(2),rr,rr);
	painter->drawEllipse(corners.at(3),rr,rr);

	painter->drawLine(corners.at(0),corners.at(1));
	painter->drawLine(corners.at(1),corners.at(2));
	painter->drawLine(corners.at(2),corners.at(3));
	painter->drawLine(corners.at(3),corners.at(0));

	painter->restore();
}

