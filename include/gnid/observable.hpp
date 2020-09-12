#ifndef OBSERVABLE_HPP
#define OBSERVABLE_HPP

#include <functional>
#include <memory>

#include "gnid/observer.hpp"
#include "gnid/subscription.hpp"

namespace gnid
{

/**
 * \brief An observable object that yields the given type
 *
 * \details
 *     An observable is an interface for callback functions. The reason
 *     observables are used is that when using raw callback functions, it is
 *     possible that the object that created the function will go out of scope.
 *     If the callback captured the "this" pointer of that object, it could
 *     accidentally cause segmentation faults. To prevent this issue, an
 *     observer object is created instead to wrap the function. This way, when
 *     the observable goes out of scope, the observable objects it owns do as
 *     well, and are destroyed, so the observable knows not to call the function
 *     again.
 *
 *     Observables are also useful because they avoid the limitations of
 *     std::shared_ptr with multiple inheritance. It is possible to do this, but
 *     it is a somewhat "hacky" solution in my opinion. Having a separate
 *     observable object provides a cleaner interface to the events emitted by
 *     an object.
 *
 *     This observable class is very loosely inspired by RxJS. Please note that
 *     I am not an expert in RxJS at the time of this writing, which is why I
 *     specified _very loosely_. Essentially, this class is instantiated with a
 *     subscribe function. When an Observer calls the Observable::subscribe
 *     function, the subscribe function is called, passing in the Observer. The
 *     subscribed function can do anything it likes with the Observer, for
 *     example, store it in a list to notify later. The observable class really
 *     is just a wrapper around a function.
 */
template<typename T>
class Observable
{
public:
    /**
     * \brief
     *     The type of function called when an observer subscribes to the
     *     observable
     */
    typedef std::function<void(std::shared_ptr<Observer<T>>)>
        SubscribeHandler;

    /**
     * \brief Create an observable with the given handlers
     */
    Observable(SubscribeHandler subscribe);

    /**
     * \brief Subscribe the given observer to the observable
     */
    void subscribe(std::shared_ptr<Observer<T>> observer);

    /**
     * \brief Unsubscribe the given observer from the observable
     */
    void unsubscribe(std::shared_ptr<Observer<T>> observer);

private:
    SubscribeHandler subscribe_;
};

template<typename T>
Observable<T>::Observable(
        Observable<T>::SubscribeHandler subscribe)
    : subscribe_(subscribe)
{
}

template<typename T>
void Observable<T>::subscribe(
        std::shared_ptr<Observer<T>> observer)
{
    subscribe_(observer);
}

} /* namespace */

#endif
