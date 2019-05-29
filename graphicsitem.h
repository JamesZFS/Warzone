#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include <QGraphicsItem>

struct b2Vec2;

class GraphicsItem : public QGraphicsItem
{
public:
    GraphicsItem() = default;

    void setB2Pos(const b2Vec2 &b_pos);
};

#endif // GRAPHICSITEM_H
