/*
 * PageMapper.cpp
 *
 *  Created on: 1 abr. 2020
 *      Author: claud
 */

#include "PageViewer.h"
#include "PageMapper.h"

PageMapper::PageMapper(PageViewer *parent) {
	parentViewer = parent;
	parentViewer->setMapper(this);

	corners.fill(QPoint(0,0), 4);
}

PageMapper::~PageMapper() {
	parentViewer = nullptr;
}

QPoint PageMapper::getCornerPosition(int index) {
	if (index < 0 || index > 3) {
		return QPoint();
	}
	return corners.at(index);
}

int PageMapper::indexOf(QPoint pos, double ratio) {

	for (int i = 0; i < 4; i++) {
		int x = pos.x() - corners.at(i).x();
		int y = pos.y() - corners.at(i).y();
		double distance = sqrt(x*x + y*y);
		if (distance < 5/ratio) {
			return i;
		}
	}

	return -1;
}

void PageMapper::setCornerPosition(int index, QPoint pos) {
	if (index < 0 || index > 3) {
		return;
	}

	corners[index] = pos;
	emit cornerChanged();
}
