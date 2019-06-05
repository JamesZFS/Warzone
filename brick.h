#ifndef BRICK_H
#define BRICK_H

#include "actor.h"

#include <Box2D/Box2D.h>

struct BrickDef
{
    b2BodyType type = b2_dynamicBody;
    int life = 100;
    qreal density;
    b2Vec2 position = {0, 0};
    b2Shape *shape = nullptr;   // only circle and polygons are allowed
    QColor color = Qt::gray;

    void rand()
    {
        type = randint(0, 2) == 0 ? b2_dynamicBody : b2_staticBody;
        life = randint(10, 200);
        density = randf(0.5, 4.0);
        position.Set(randf(-0.8*GameConsts::world_width, +0.8*GameConsts::world_width),
                     randf(-0.8*GameConsts::world_height, +0.8*GameConsts::world_height));
        color = QColor(randf(150, 250), randf(150, 250), randf(150, 250));
    }
};

// breakable brick
class Brick : public AnimateActor
{
    Q_OBJECT
public:
    // body has defined its fixture before calling this
    Brick(b2Body *body, int life, const QColor &color);

    // helpful creators: (body already set its type and position)
    // angle is in degrees
    static Brick *createBlock(b2Body *body, qreal size, const QColor &color = Qt::gray);
    static Brick *createCircle(b2Body *body, qreal rad, const QColor &color = Qt::gray);
    static Brick *createTriangle(b2Body *body, qreal size, const QColor &color = Qt::gray);
    static Brick *createU(b2Body *body, qreal width, qreal height, const QColor &color = Qt::gray);

protected:
    QColor m_color;

    // QGraphicsItem interface
public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // BRICK_H
