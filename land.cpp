#include "land.h"
#include "defs.h"
#include <Box2D/Box2D.h>
#include <QPainter>

Land::Land(b2Body *land_body) : Actor(land_body)
{
    Q_ASSERT(m_body->GetFixtureList());
    m_body->SetType(b2_staticBody); // land cannot move
    m_bbox = findBodyBound();
    m_bbox += QMarginsF(1, 1, 1, 1);    // adjust
    qDebug() << "LAND BOX:" << m_bbox;
}

void Land::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    static QPixmap texture(":/res/grasstexture.jpg");
    static QBrush brush;
    brush.setTexture(texture);
    painter->setBrush(brush);
    drawFixtures(painter, m_body->GetFixtureList());
}

