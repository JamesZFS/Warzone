#include "explosioneffect.h"

#include <QPainter>
#include <QPixmap>
#include <QtMath>

QPixmap getTransparentPixmap(const QPixmap &img, int alpha)
{
    if (alpha >= 255 || alpha < 0) return img;
    QPixmap temp(img.size());
    temp.fill(Qt::transparent);
    QPainter p(&temp);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawPixmap(0, 0, img);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(temp.rect(), QColor(0, 0, 0, alpha));
    p.end();
    return temp;
}

ExplosionEffect::ExplosionEffect(qreal radius, qreal duration) :
    m_radius(radius), m_anim(this, "t")
{
    m_bbox = QRectF(-m_radius, -m_radius, 2 * m_radius, 2 * m_radius);
    m_anim.setStartValue(0.0);
    m_anim.setEndValue(1.5);
    m_anim.setDuration(duration);
    connect(&m_anim, &QAbstractAnimation::finished, this, [this](){emit animationFinished();});
}

void ExplosionEffect::startAnimation()
{
    m_anim.start();
}

void ExplosionEffect::stopAnimation()
{
    m_anim.stop();
}

QRectF ExplosionEffect::boundingRect() const
{
    return m_bbox;
}

void ExplosionEffect::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    qreal rad;
    int alpha;
    if (t() <= 1.0) {
        rad = qSqrt(t()) * radius() ;
        alpha = 255;
    }
    else {
        rad = radius();
        alpha = 255 * 2 * (1.5 - t());
    }
    _paint(painter, rad, alpha);
//    QRadialGradient g(QPointF(), rad);
//    g.setSpread(QGradient::ReflectSpread);
//    g.setColorAt(0, QColor(255, 255, 255, alpha));
//    g.setColorAt(0.7, QColor(240, 230, 150, alpha));
//    g.setColorAt(1.0, QColor(250, 200, 50, alpha));
//    painter->setPen(QPen(Qt::red, 2));
//    painter->setBrush(g);
//    painter->drawEllipse(QPointF(), rad, rad);
}

BazookaExplosionEffect::BazookaExplosionEffect(qreal radius) :
    ExplosionEffect(radius, 250)
{

}

void BazookaExplosionEffect::_paint(QPainter *painter, qreal rad, int alpha)
{
    static QPixmap img(":/img/explosion2.png");
    painter->drawPixmap(-rad, -rad, 2 * rad, 2 * rad, getTransparentPixmap(img, alpha));
}
