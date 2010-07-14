#include "graphicsview.h"

GraphicsView::GraphicsView():
    QGraphicsView(new QGraphicsScene()),
    viewport(0)
{
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    setOptimizationFlags(QGraphicsView::DontSavePainterState);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setFrameShape(QFrame::NoFrame);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    viewport = new ViewportItem();
    scene()->addItem(viewport);
}

GraphicsView::ViewportItem *viewportItem() const
{
    return viewport;
}

 protected:
     void resizeEvent(QResizeEvent *e)
     {
         QGraphicsView::resizeEvent(e);
         setUpdatesEnabled(false);

         if (!viewport)
             return;

         QRectF rect(QPointF(0, 0), size());
         scene()->setSceneRect(rect);

         viewport->setGeometry(rect);
         setUpdatesEnabled(true);
         update();
     }

 private:
     ViewportItem *viewport;
 };
