/*
 * PageMapper.h
 *
 *  Created on: 1 abr. 2020
 *      Author: claud
 */

#ifndef SRC_PAGEMAPPER_H_
#define SRC_PAGEMAPPER_H_

#include <QtWidgets/QtWidgets>

class PageViewer;

class PageMapper: public QObject {
	Q_OBJECT

	public:
		PageMapper(PageViewer *parent = nullptr);
		virtual ~PageMapper();

		int indexOf(QPoint pos, double ratio);
		void setCornerPosition(int index, QPoint pos);
		QPoint getCornerPosition(int index);

	signals:
		void cornerChanged();

	private:
		PageViewer *parentViewer;
		QVector<QPoint> corners;
};



#endif /* SRC_PAGEMAPPER_H_ */
