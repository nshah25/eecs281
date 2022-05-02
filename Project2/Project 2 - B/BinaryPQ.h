// Project identifier: 19034C8F3B1196BF8E0C6E1C0F973D2FD550B88F

#ifndef BINARYPQ_H
#define BINARYPQ_H


#include <algorithm>
#include <vector>
#include "Eecs281PQ.h"

using namespace std;

// A specialized version of the 'heap' ADT implemented as a binary heap.
template<typename TYPE, typename COMP_FUNCTOR = std::less<TYPE>>
class BinaryPQ : public Eecs281PQ<TYPE, COMP_FUNCTOR> {
    // This is a way to refer to the base class object.
    using BaseClass = Eecs281PQ<TYPE, COMP_FUNCTOR>;

public:
    // Description: Construct an empty heap with an optional comparison functor.
    // Runtime: O(1)
    explicit BinaryPQ(COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp } {
    }

    // Description: Construct a heap out of an iterator range with an optional
    //              comparison functor.
    // Runtime: O(n) where n is number of elements in range.
    template<typename InputIterator>
    BinaryPQ(InputIterator start, InputIterator end, COMP_FUNCTOR comp = COMP_FUNCTOR()) :
        BaseClass{ comp }, data{ start, end }
    {
        updatePriorities();
    }

    // Description: Destructor doesn't need any code, the data vector will
    //              be destroyed automaticslly.
    virtual ~BinaryPQ() {
    }

    // Description: Assumes that all elements inside the heap are out of order and
    //              'rebuilds' the heap by fixing the heap invariant.
    // Runtime: O(n)
    virtual void updatePriorities()
    {
        for (size_t i = (data.size() / 2); i > 0; --i)
        {
            fixDown((int)i);
        }
    }

    // Description: Add a new element to the heap.
    // Runtime: O(log(n))
    virtual void push(const TYPE& val)
    {
        data.push_back(val);
        fixUp((int)data.size());
    }

    // Description: Remove the most extreme (defined by 'compare') element from
    //              the heap.
    // Note: We will not run tests on your code that would require it to pop an
    // element when the heap is empty. Though you are welcome to if you are
    // familiar with them, you do not need to use exceptions in this project.
    // Runtime: O(log(n))
    virtual void pop()
    {
        data[0] = data.back();
        data.pop_back();
        fixDown(1);
    }

    // Description: Return the most extreme (defined by 'compare') element of
    //              the heap.  This should be a reference for speed.  It MUST be
    //              const because we cannot allow it to be modified, as that
    //              might make it no longer be the most extreme element.
    // Runtime: O(1)
    virtual const TYPE& top() const
    {
        return getElement(1);
    }

    // Description: Get the number of elements in the heap.
    // Runtime: O(1)
    virtual std::size_t size() const 
    {
        return data.size();
    }

    // Description: Return true if the heap is empty.
    // Runtime: O(1)
    virtual bool empty() const 
    {
        if ((int)data.size() > 0) 
        {
            return false;
        }
        return true;
    }

private:
    std::vector<TYPE> data;

    TYPE& getElement(int i)
    {
        return data[(std::size_t)i - 1];
    }

    const TYPE& getElement(int i) const 
    {
        return data[(std::size_t)i - 1];
    }

    void fixUp(int x)
    {
        while (x > 1 && this->compare(getElement(x / 2), getElement(x))) 
        {
            swap(getElement(x), getElement(x / 2));
            x /= 2;
        }
    }

    void fixDown(int x) 
    {
        while (2 * x <= int(data.size()))
        {
            int i = 2 * x;

            if (i < int(data.size()) && this->compare(getElement(i), getElement(i + 1)))
            {
                ++i;
            }

            if (!this->compare(getElement(x), getElement(i)))
            {
                break;
            }

            swap(getElement(x), getElement(i));
            x = i;
        }
    }

}; // BinaryPQ


#endif // BINARYPQ_H
