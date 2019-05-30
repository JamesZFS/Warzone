#include "land.h"
#include "defs.h"
#include <Box2D/Box2D.h>
#include <QPainter>

Land::Land(b2Body *land_body) : Actor(land_body)
{
    Q_ASSERT(m_body->GetFixtureList());
    m_body->SetType(b2_staticBody); // land cannot move
    for (auto *fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        fixture->SetFriction(0.8);
        auto b_polygon = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
        Q_ASSERT(b_polygon);
        auto polygon = fromB2Polygon(*b_polygon);
        m_bbox = m_bbox.united(polygon.boundingRect());
    }
    m_bbox += QMarginsF(1, 1, 1, 1);    // adjust
}

void Land::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int i = 0;
    for (auto *fixture = m_body->GetFixtureList(); fixture; fixture = fixture->GetNext(), ++i) {
        auto b_polygon = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
        Q_ASSERT(b_polygon);

        auto polygon = fromB2Polygon(*b_polygon);
        QColor color(10, 180 + 20 * i, 20);
        painter->setBrush(color);
        painter->drawPolygon(polygon);
    }
}
