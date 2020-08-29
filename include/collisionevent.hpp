#ifndef COLLISIONEVENT_HPP
#define COLLISIONEVENT_HPP value

#include <memory>
#include "event.hpp"

namespace gnid
{

enum CollisionEventType
{
    COLLISION_ENTERED,
    COLLISION_STAYED,
    COLLISION_EXITED
};

class CollisionEvent : public Event
{
public:
    CollisionEvent(
            const std::shared_ptr<Node> source,
            const Collision collision,
            CollisionEventType type)
        : Event::Event(source), collision_(collision), type_(type)
    {
    }

    const Collision &collision() { return collision_; }
    const CollisionEventType &type() { return type_; }
private:
    const Collision collision_;
    const CollisionEventType type_;
};

} /* namespace */

#endif /* ifndef COLLISIONEVENT_HPP */
