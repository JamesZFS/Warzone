#ifndef LAND_H
#define LAND_H

#include <QGraphicsItem>
//#include <Box2D/Box2D.h>
class b2Body;
class b2Fixture;
struct b2Vec2;

class GameSystem;

class Land : public QGraphicsItem
{
    friend class GameSystem;

protected:
    explicit Land(b2Body *land_body_);
    ~Land() = default;

private:
    b2Body* const body;

    QRectF b_box;   // bounding rect

public:
    void setB2Pos(const b2Vec2 &b_pos);

    // QGraphicsItem interface
    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // LAND_H
