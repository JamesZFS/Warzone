#ifndef CONTACTLISTENER_H
#define CONTACTLISTENER_H

#include "defs.h"
#include "weapon.h"
#include <Box2D/Box2D.h>


// using Observer design pattern
class ContactListener : public b2ContactListener
{
public:
    // b2ContactListener interface
    void BeginContact(b2Contact *contact) override;
};

#endif // CONTACTLISTENER_H
