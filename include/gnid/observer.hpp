#ifndef SUBSCRIBER_HPP
#define SUBSCRIBER_HPP

#include <functional>
#include <memory>

namespace gnid
{

/**
 * \brief An object capable of subscribing to an Observable
 *
 * \details
 *     Observers can be used in a similar manner to callback functions. The main
 *     reason the observer is in a separate object is so that the lifetime of
 *     the function can be matched with an owning node, and to avoid the
 *     complications of using shared pointers with multiple inheritance. This
 *     way, when the owner of the observer is destroyed, the Observable the
 *     observer is subscribed to knows to remove the observer from the list of
 *     observables. Observers must be used alongside the Observable::subscribe
 *     function. Then, the observable will call the next() function of the
 *     observer as it sees fit.
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

