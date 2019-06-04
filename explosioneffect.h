#ifndef EXPLOSIONEFFECT_H
#define EXPLOSIONEFFECT_H

#include <QGraphicsObject>
#include <QPropertyAnimation>


class ExplosionEffect : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT)
public:
    ExplosionEffect(qreal radius, qreal duration);
    void startAnimation();
    void stopAnimation();
    qreal radius() const;

protected:
    qreal t() const;
    void setT(qreal t);
    virtual void _paint(QPainter *painter, qreal rad, int alpha) = 0;

signals:
    void animationFinished();

private:
    const qreal m_radius;
    qreal m_t;  // ranging from 0 to 1
    QPropertyAnimation m_anim;
    QRectF m_bbox;

    // QGraphicsItem interface
public:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

inline qreal ExplosionEffect::t() const
{
    return m_t;
}

inline void ExplosionEffect::setT(qreal t)
{
    m_t = t;
    update();
}

inline qreal ExplosionEffect::radius() const
{
    return m_radius;
}

class BazookaExplosionEffect : public ExplosionEffect
{
public:
    BazookaExplosionEffect(qreal radius);


    // ExplosionEffect interface
protected:
    void _paint(QPainter *painter, qreal rad, int alpha) override;
};

#endif // EXPLOSIONEFFECT_H
