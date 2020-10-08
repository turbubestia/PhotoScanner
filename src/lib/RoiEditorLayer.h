/*
 * RoiEditor.h
 *
 *  Created on: 28 jun. 2020
 *      Author: claud
 */

#ifndef SRC_LIBS_ROIEDITORLAYER_H_
#define SRC_LIBS_ROIEDITORLAYER_H_

#include <QtWidgets/QtWidgets>

#include "AbstractLayer.h"

class RoiEditorLayer : public AbstractLayer {
	Q_OBJECT

	public:
		RoiEditorLayer();
		virtual ~RoiEditorLayer();

		int indexOf(QPoint pos, double ratio);
        void setCornerPosition(int index, QPoint pos);
        QPoint getCornerPosition(int index);

		bool mousePressEvent(QMouseEvent *event) override;
		bool mouseReleaseEvent(QMouseEvent *event) override;
		bool mouseMoveEvent(QMouseEvent *event) override;
		void render(QPainter *painter) override;

	signals:
		void changed();

	private:
		QVector<QPointF> corners;

		bool drag;
		QPoint lastDragPoint;

		int corner;

		double r;
};

#endif /* SRC_LIBS_ROIEDITORLAYER_H_ */
