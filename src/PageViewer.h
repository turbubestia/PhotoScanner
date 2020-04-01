/*
 * PageViewer.h
 *
 *  Created on: 13 oct. 2019
 *      Author: claud
 */

#ifndef SRC_PAGEVIEWER_H_
#define SRC_PAGEVIEWER_H_

#include <QtWidgets/QtWidgets>

class PageMapper;

class PageViewer: public QWidget {
	Q_OBJECT

	public:
		PageViewer(QWidget *parent = nullptr);
		virtual ~PageViewer();

		void setMapper(PageMapper *corners);
		void setPixmap(const QPixmap &pixmap);
		const QPixmap &getPixmap();

		virtual QSize sizeHint() const;

	protected:
		void mouseMoveEvent(QMouseEvent *event) override;
		void mousePressEvent(QMouseEvent *event) override;
		void mouseReleaseEvent(QMouseEvent *event) override;
	    void paintEvent(QPaintEvent *event) override;

	private:
	    QPoint mapWidgetToImage(QPoint pos);
	    QPoint mapImageToWidget(QPoint pos);

	private:
	    QPixmap page;
	    double dpi;
	    double ratio;
	    double zoom;

	    PageMapper *corners;
	    int moveCornerIndex;
	    QPointF lastMousePos;
	    QPointF lastMousePosUn;
};

#endif /* SRC_PAGEVIEWER_H_ */
