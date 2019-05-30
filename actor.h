#ifndef ACTOR_H
#define ACTOR_H

#include <QGraphicsObject>

struct b2Vec2;

class b2World;
class b2Body;
class b2PolygonShape;
typedef float float32;

class Actor : public QGraphicsObject
{
public:
    Actor(b2Body *body);

    ~Actor() = default;

    void setB2Pos(const b2Vec2 &b_pos);
    void setB2Angle(const float32 angle);

    QPointF mapB2Pos(const b2Vec2 &b_pos) const;  // map b2Vec to screen coord
    QPolygonF fromB2Polygon(const b2PolygonShape &b2_poly) const;

protected:
    /**
     * Physical body, enpowered by LiquidFun™ backend
     * the ownership is taken by world
     */
    b2Body* const m_body;

    QRectF m_bbox; // bounding box

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override
    { return m_bbox; }

    virtual void advance(int phase) override;

    b2Body *body() const;
};

#endif // ACTOR_H
