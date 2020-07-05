#ifndef BOX_HPP
#define BOX_HPP

#include <cassert>

namespace gnid
{

template<int N, typename T>
class Box
{
public:
    Box() {}

    /* Adds the point to the box and returns the box for convenience. */
    void add(const Vector<N, T> v);

    /* Adds the given box to the box and returns this box for convenience. */
    void add(const Box other);
    bool overlaps(const Box other) const;
    bool contains(const Box other) const;
    bool contains(const Vector3f other) const;

    /* Transform the box by the given matrix, resulting in a new box. */
    void transform(Matrix<N + 1, N + 1, T> matrix);

    const Vector<N, T> &min() const { assert(count() > 0); return min_; }
    const Vector<N, T> &max() const { assert(count() > 0); return max_; }
    Vector<N, T> center() const;
    const int &count() const { return count_; }

    void clear() { count_ = 0; }

    /******** Collider shape methods. ********/
    const Box<N, T> &box() const;
    Vector<N, T> support(const Vector<N, T> d) const;

private:
    Vector<N, T> min_;
    Vector<N, T> max_;
    int count_ = 0;
};

typedef Box<3, float> Box3f;
typedef Box<4, float> Box4f;

template<int N, typename T>
void Box<N, T>::add(const Vector<N, T> v)
{
    if(count_ == 0)
    {
        min_ = v;
        max_ = v;
    }
    else
    {
        for(int i = 0; i < N; i ++)
        {
            if(v[i] < min_[i])
                min_[i] = v[i];
            if(v[i] > max_[i])
                max_[i] = v[i];
        }
    }
    count_ ++;
}

template<int N, typename T>
void Box<N, T>::add(const Box<N, T> box)
{
    if(count_ == 0)
    {
        min_ = box.min();
        max_ = box.max();
    }
    else
    {
        for(int i = 0; i < N; i ++)
        {
            if(box.min()[i] < min_[i])
                min_[i] = box.min()[i];
            if(box.max()[i] > max_[i])
                max_[i] = box.max()[i];
        }
    }
    count_ ++;
}

template<int N, typename T>
bool Box<N, T>::overlaps(const Box other) const
{
    assert(count() > 0);
    return min() <= other.max() && min() >= other.min()
        || max() <= other.max() && max() >= other.min();
}

template<int N, typename T>
bool Box<N, T>::contains(const Box other) const
{
    assert(count() > 0);
    return min() <= other.min() && max() >= other.max();
}

template<int N, typename T>
bool Box<N, T>::contains(const Vector3f other) const
{
    assert(count() > 0);
    return min() <= other && max() >= other;
}

template<int N, typename T>
void Box<N, T>::transform(Matrix<N + 1, N + 1, T> matrix)
{
    assert(count() > 0);
    /* Calculate the number of points in the shape, 2^n. */
    const int pointCount = 1 << N;

    Vector<N, T> diff = max() - min();

    Vector<N + 1, T> newMin = matrix * min().homo();

    /* Calculate the new basis. */
    Vector<N, T> basis[N];
    for(int i = 0; i < N; i ++)
    {
        basis[i] = matrix.column(i) * diff[i];
    }

    /* Add all the points in the transformed box. */
    int oldCount = count();
    clear();

    for(int i = 0; i < pointCount; i ++)
    {
        Vector<N + 1, T> point = newMin;

        /* Calculate the point's position. */
        for(int j = 0; j < N; j ++)
        {
            if((i >> j) & 1 == 1)
            {
                point += basis[i];
            }
        }

        add(point);
    }

    count_ = oldCount;
}

template<int N, typename T>
Vector<N, T> Box<N, T>::center() const
{
    assert(count() > 0);
    return (min() + max()) * 0.5f;
}

template<int N, typename T>
const Box<N, T> &Box<N, T>::box() const
{
    return *this;
}

template<int N, typename T>
Vector<N, T> Box<N, T>::support(const Vector<N, T> d) const
{
    Vector<N, T> ret = min();
    T retDot = d.dot(min());

    const int pointCount = 1 << N;
    Vector<N, T> diff = max() - min();

    for(int i = 1; i < pointCount; i ++)
    {
        Vector<N + 1, T> point = min();

        /* Calculate the point's position. */
        for(int j = 0; j < N; j ++)
        {
            if((i >> j) & 1 == 1)
            {
                point[i] += diff[i];
            }
        }

        T dot = d.dot(point);
        if(dot > retDot)
        {
            ret = point;
            retDot = d.dot(point);
        }
    }

    return ret;
}

} /* namespace */


#endif /* ifndef BOX_HPP */

