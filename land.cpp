#include "land.h"
#include "defs.h"
#include <Box2D/Box2D.h>
#include <QPainter>

Land::Land(b2Body *land_body) : Actor(land_body)
{
    Q_ASSERT(m_body->GetFixtureList());
    m_body->SetType(b2_staticBody); // land cannot move
    m_bbox = findBodyBound();
    qDebug() << "LAND BOX:" << m_bbox;
    m_bbox += QMarginsF(1, 1, 1, 1);    // adjust
}

void Land::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    static QPixmap texture(":/img/grasstexture.jpg");
    static QBrush brush;
    brush.setTexture(texture);
    painter->setBrush(brush);
    drawFixtures(painter, m_body->GetFixtureList());
}

Land *Land::create(b2Body *body, qreal w, qreal h)
{
    b2PolygonShape shape;
    // four edges:
    shape.SetAsBox(w, 1, b2Vec2(0, +h - 1), 0);
    body->CreateFixture(&shape, 0);
    shape.SetAsBox(w, 1, b2Vec2(0, -h + 1), 0);
    body->CreateFixture(&shape, 0);
    shape.SetAsBox(1, h, b2Vec2(-w + 1, 0), 0);
    body->CreateFixture(&shape, 0);
    shape.SetAsBox(1, h, b2Vec2(+w - 1, 0), 0);
    body->CreateFixture(&shape, 0);

    // land: (shape like "M")
    b2Vec2 pl[4] = { b2Vec2(-w, -h/2), b2Vec2(-4*w/5, 0), b2Vec2(-2*w/5, 0), b2Vec2(-w/5, -h/2) };
    b2Vec2 pr[4] = { b2Vec2(+w, -h/2), b2Vec2(+4*w/5, 0), b2Vec2(+2*w/5, 0), b2Vec2(+w/5, -h/2) };
    shape.Set(pl, 4);
    body->CreateFixture(&shape, 0);
    shape.Set(pr, 4);
    body->CreateFixture(&shape, 0);

    // bottom
    shape.SetAsBox(w, h/4, b2Vec2(0, -3*h/4), 0);
    body->CreateFixture(&shape, 0);

    return new Land(body);
}
