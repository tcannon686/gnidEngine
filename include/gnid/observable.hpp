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
