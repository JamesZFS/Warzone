#include "graphicsitem.h"
#include <Box2D/Box2D.h>

void GraphicsItem::setB2Pos(const b2Vec2 &b_pos)
{
    setPos(b_pos.x, b_pos.y);
}
