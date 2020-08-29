#ifndef EVENT_HPP
#define EVENT_HPP

#include <memory>

namespace gnid
{

class Node;

class Event
{
public:
    Event(const std::shared_ptr<Node> source)
        : source_(source)
    {
    }

    virtual ~Event()
    {
    }

    const std::shared_ptr<Node> &source() { return source_; }
private:
    const std::shared_ptr<Node> source_;
};

} /* namespace */

#endif /* ifndef EVENT_HPP */
