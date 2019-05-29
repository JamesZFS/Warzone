#ifndef LAND_H
#define LAND_H

#include "graphicsitem.h"
//#include <Box2D/Box2D.h>
class b2Body;
class b2Fixture;

class GameSystem;

class Land : public GraphicsItem
{
    friend class GameSystem;

protected:
    explicit Land(b2Body *land_body_);
    ~Land() = default;

private:
    b2Body* const body;

    QRectF b_box;   // bounding rect

public:
    // QGraphicsItem interface
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // LAND_H
