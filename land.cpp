#include "land.h"
#include "defs.h"
#include <Box2D/Box2D.h>
#include <QPainter>

Land::Land(b2Body *land_body_) : Actor(land_body_, 0, 0.5)
{
    for (auto *fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        auto b_polygon = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
        Q_ASSERT(b_polygon);
        auto polygon = fromB2Polygon(*b_polygon);
        b_box = b_box.united(polygon.boundingRect());
    }
    b_box += QMarginsF(1, 1, 1, 1);
}

QRectF Land::boundingRect() const
{
    return b_box;
}

void Land::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int i = 0;
    for (auto *fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext(), ++i) {
        auto b_polygon = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
        Q_ASSERT(b_polygon);

        auto polygon = fromB2Polygon(*b_polygon);
        QColor color(10, 180 + 20 * i, 20);
        painter->setBrush(color);
        painter->drawPolygon(polygon);
    }
}
