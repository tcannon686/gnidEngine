#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP

#include <functional>
#include <memory>

namespace gnid
{

/**
 * \brief An object capable of subscribing to an Observable
 */
template<typename T>
class Observer
{
public:
    typedef std::function<void(T)> NextHandler;

    Observer(NextHandler next);

    void next(T value);

private:
    NextHandler next_;
};

template<typename T>
Observer<T>::Observer(NextHandler next)
    : next_(next)
{
}

template<typename T>
void Observer<T>::next(T value)
{
    next_(value);
}

} /* namespace */

#endif
