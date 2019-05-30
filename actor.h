#ifndef ACTOR_H
#define ACTOR_H

#include <QGraphicsItem>

struct b2Vec2;

class b2Body;
class b2PolygonShape;

class Actor : public QGraphicsItem
{
public:
    Actor(b2Body *body_, qreal damping, qreal friction);

    ~Actor() = default;

    void setB2Pos(const b2Vec2 &b_pos);

    QPointF mapB2Pos(const b2Vec2 &b_pos) const;  // map b2Vec to screen coord

    QPolygonF fromB2Polygon(const b2PolygonShape &b2_poly) const;

protected:
    /**
     * Physical body, enpowered by LiquidFun™ backend
     * the ownership is taken by world
     */
    b2Body* const body;

    QRectF b_box; // bounding box

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override
    { return b_box; }
};

#endif // ACTOR_H
