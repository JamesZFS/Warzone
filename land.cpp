#include "land.h"
#include "defs.h"
#include <Box2D/Box2D.h>
#include <QPainter>

Land::Land(b2Body *land_body_) : body(land_body_)
{
    Q_ASSERT(land_body_->GetFixtureList());
    float32 x_min = 1e10, x_max = -1e10, y_min = 1e10, y_max = -1e10;
    for (auto *fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext()) {
        auto b_polygon = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
        Q_ASSERT(b_polygon);
        for (int i = 0; i < b_polygon->GetVertexCount(); ++i) {    // find bound
            auto &v = b_polygon->GetVertex(i);
            x_min = std::min(x_min, v.x);
            x_max = std::max(x_max, v.x);
            y_min = std::min(y_min, v.y);
            y_max = std::max(y_max, v.y);
        }
    }
    b_box = QRectF(x_min, y_min, x_max - x_min, y_max - y_min);
}

void Land::setB2Pos(const b2Vec2 &b_pos)
{
    setPos(b_pos.x, b_pos.y);
}

QRectF Land::boundingRect() const
{
    return b_box;
}

void Land::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    int i = 0;
    for (auto *fixture = body->GetFixtureList(); fixture; fixture = fixture->GetNext(), ++i) {
        QPolygonF polygon;
        QColor color(10, 180 + 20 * i, 20);

        auto b_polygon = dynamic_cast<b2PolygonShape*>(fixture->GetShape());
        Q_ASSERT(b_polygon);
        for (int j = 0; j < b_polygon->GetVertexCount(); ++j) {  // convert to QPolygon
            auto &v = b_polygon->GetVertex(j);
            polygon << QPointF(v.x, v.y);
        }

        painter->setBrush(color);
        painter->drawPolygon(polygon);
    }
}
