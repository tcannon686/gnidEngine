#ifndef EVENT_HPP
#define EVENT_HPP

#include <memory>

namespace gnid
{

class Node;
using namespace std;

class Event
{
public:
    Event(const shared_ptr<Node> source)
        : source_(source)
    {
    }

    virtual ~Event()
    {
    }

    const shared_ptr<Node> &source() { return source_; }
private:
    const shared_ptr<Node> source_;
};

} /* namespace */

#endif /* ifndef EVENT_HPP */
