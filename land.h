#ifndef LAND_H
#define LAND_H

#include "actor.h"
//#include <Box2D/Box2D.h>
class b2Body;
class b2Fixture;

class GameSystem;

class Land : public Actor
{
public:
    explicit Land(b2Body *land_body_);

    // QGraphicsItem interface
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // LAND_H
