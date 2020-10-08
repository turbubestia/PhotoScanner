/*
 * AbstractOverlay.h
 *
 *  Created on: Jun 8, 2020
 *      Author: claud
 */

#ifndef SRC_LIBS_ABSTRACTLAYER_H_
#define SRC_LIBS_ABSTRACTLAYER_H_

#include <QtWidgets/QtWidgets>

class LayerGraphicWidget;

class AbstractLayer  : public QObject{
    Q_OBJECT

    public:
        AbstractLayer();
        virtual ~AbstractLayer();

        bool isVisible() { return visible; }

        LayerGraphicWidget *parent() {
            return _parent;
        }

        QSize size() {
            return QSize(_width, _height);
        }

        virtual bool mouseDoubleClickEvent(QMouseEvent *event) ;
        virtual bool mousePressEvent(QMouseEvent *event) ;
        virtual bool mouseReleaseEvent(QMouseEvent *event) ;
        virtual bool mouseMoveEvent(QMouseEvent *event) ;
        virtual void render(QPainter *painter) = 0;
        virtual void resize(QSize size);
        virtual void resizeToCanvas();

        void setParent(LayerGraphicWidget *parent = nullptr) {
            this->_parent = parent;
        }

        void setVisible(bool en) {
        	visible = en;
        }

    protected:
        int _width;
        int _height;

    private:
        bool visible;
        LayerGraphicWidget *_parent;
};

#endif /* SRC_LIBS_ABSTRACTLAYER_H_ */
