#include "contactlistener.h"

void ContactListener::BeginContact(b2Contact *contact)
{
//    qDebug() << "BeginContact()";

    auto body_A = contact->GetFixtureA()->GetBody();
    auto body_B = contact->GetFixtureB()->GetBody();
    auto actor_A = (Actor*)(body_A->GetUserData());
    auto actor_B = (Actor*)(body_B->GetUserData());
//    Q_ASSERT(actor_A && actor_B);

    // contact weapon detection
    auto weapon_A = dynamic_cast<ContactWeapon*>(actor_A);
    auto weapon_B = dynamic_cast<ContactWeapon*>(actor_B);
    if (!weapon_A && !weapon_B) return;     // an unrelated contact
    Q_ASSERT(weapon_A != weapon_B);

    qreal v_approach = (body_A->GetLinearVelocity() - body_B->GetLinearVelocity()).Length();
    if (weapon_A && v_approach >= weapon_A->threasholdV()) weapon_A->trigger();
    if (weapon_B && v_approach >= weapon_B->threasholdV()) weapon_B->trigger();
}
