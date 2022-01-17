/*
 * PixelLayer.h
 *
 *  Created on: Jun 10, 2020
 *      Author: claud
 */

#ifndef SRC_LIBS_PIXELLAYER_H_
#define SRC_LIBS_PIXELLAYER_H_

#include <QtWidgets/QtWidgets>

#include "AbstractLayer.h"

class PixelLayer: public AbstractLayer {
    public:
        PixelLayer();
        virtual ~PixelLayer();

        void clear();
        void render(QPainter *painter) override;
        void resize(QSize size) override;
        void resizeToCanvas() override;
        void setImage(const QImage &image);
        const QImage &image();

    private:
        QImage _image;
};

#endif /* SRC_LIBS_PIXELLAYER_H_ */
