#include "initializer.h"
#include "defs.h"
#include "soldier.h"
#include "land.h"
#include "brick.h"

#include <Box2D/Box2D.h>
#include <QBrush>

static const auto w = GameConsts::world_width, h = GameConsts::world_height;

Initializer::Initializer(b2World *world) : m_world(world)
{
}

void FortressInitializer::initLand(Land *&land)
{
    Q_ASSERT(!land);
    // new land instance
    b2BodyDef def;
    def.position.SetZero();
    auto body = m_world->CreateBody(&def);
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

    // landscape: (shape like "M")
    static const b2Vec2 pl[5] = { b2Vec2(-w, -h/5), b2Vec2(-4*w/5, 0), b2Vec2(-2*w/5, 0), b2Vec2(-w/5, -h/2), b2Vec2(-w, -h/2) };
    static const b2Vec2 pr[5] = { b2Vec2(+w, -h/5), b2Vec2(+4*w/5, 0), b2Vec2(+2*w/5, 0), b2Vec2(+w/5, -h/2), b2Vec2(+w, -h/2) };
    shape.Set(pl, 5);
    body->CreateFixture(&shape, 0);
    shape.Set(pr, 5);
    body->CreateFixture(&shape, 0);

    // bottom
    shape.SetAsBox(w, h/4, b2Vec2(0, -3*h/4), 0);
    body->CreateFixture(&shape, 0);
    land = new Land(body);
}

void FortressInitializer::initBricks(QSet<Brick *> &bricks)
{
    static const QBrush
            brush_wood(Qt::white, QPixmap(":/res/woodtexture.jpg")),
            brush_iron(Qt::white, QPixmap(":/res/ironfencetexture.jpg")),
            brush_brick(Qt::white, QPixmap(":/res/bricktexture.jpeg"));
    // create bricks
    b2BodyDef def;
    b2Body *body;
    Brick *brick;
    def.type = b2_dynamicBody;
    def.angle = b2_pi;

    // left fortress
    def.position.Set(-0.6*w, 0.3*h);
    body = m_world->CreateBody(&def);
    brick = Brick::createU(body, 0.2*w, 0.3*h, 2, &brush_iron);   // "N" shape
    brick->setCollisionCategory(e_RED);
    bricks << brick;

    def.position.Set(-0.6*w, 0.5*h);
    body = m_world->CreateBody(&def);
    brick = Brick::createU(body, 0.1*w, 0.2*h, 1.5, &brush_iron);
    brick->setCollisionCategory(e_RED);
    bricks << brick;

    // right fortress
    def.position.Set(+0.6*w, 0.3*h);
    body = m_world->CreateBody(&def);
    brick = Brick::createU(body, 0.2*w, 0.3*h, 2, &brush_wood);
    brick->setCollisionCategory(e_BLACK);
    bricks << brick;

    def.position.Set(+0.6*w, 0.5*h);
    body = m_world->CreateBody(&def);
    brick = Brick::createU(body, 0.1*w, 0.2*h, 1.5, &brush_wood);
    brick->setCollisionCategory(e_BLACK);
    bricks << brick;

    for (int i = 0; i < GameConsts::max_n_brick; ++i) {
        def.position.Set(randf(-0.3*w, 0.3*w), randf(0*h, 0.3*h));
        def.angle = randf(0, b2_pi);
        body = m_world->CreateBody(&def);
        switch (randint(0, 3)) {
        case 0:
        case 1:
            bricks << Brick::createBlock(body, randf(0.02*w, 0.05*w), &brush_brick);
            break;
        case 2:
            bricks << Brick::createCircle(body, randf(0.02*w, 0.06*w), &brush_brick);
            break;
        default:
            break;
        }
    }
}

void FortressInitializer::initUnits(QSet<RedSoldier *> &R_units, QSet<BlackSoldier *> &B_units)
{
    for (int i = 0; i < GameConsts::max_n_unit; ++i) {
        int life = 100;
        qreal size = 0.5 + i * 0.2;   // 0.5 - 1.5
        b2BodyDef def;
        // red
        def.position.Set(randf(-0.75*w, -0.45*w), randf(0, 0.7*h));
        auto body = m_world->CreateBody(&def);
        R_units << new RedSoldier(life, size, body);
        // black
        def.position.Set(randf(+0.45*w, +0.75*w), randf(0, 0.7*h));
        body = m_world->CreateBody(&def);
        B_units << new BlackSoldier(life, size, body);
    }
}
