/* simplestack.h
 *
 * Copyright (C) 2010 Alexey Naydenov
 *
 * Author: Alexey Naydenov <alexey.naydenovREMOVETHIS@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/*! \file simplestack.h 
 * Simple implementation of a stack. 
 */

#ifndef SIMPLESTACK_H
#define SIMPLESTACK_H

#include <stddef.h>

#include <boost/scoped_array.hpp>

namespace vartrace {

/*! Stores data by value.
 *
 * Tries to follow stl stack interface. */
template <typename T> class SimpleStack
{
public:
    enum {DefaultStackSize = 32};

    /*! Creates a stack with given maximum size.
     *
     * \param size maximum number of elements in the stack, default
     * value is 32.*/
    SimpleStack(unsigned size = SimpleStack::DefaultStackSize);
    virtual ~SimpleStack() {};

    /*! Checks if stack contains any elements. */
    bool empty() const;
    /*! Returns number of elements in the stack. */
    unsigned size() const;
    /*! Removes one element from the stack. */
    void pop();
    /*! Adds an element to the stack. */
    void push(const T& value);
    /*! Returns the value of the element at the head of the stack. */
    T& top();

    /*! Checks if stack is full. */
    bool full() const;
    /*! Returns maximum number of elements that can be stored in the stack. */
    unsigned maxSize() const;
    /*! Check if some errors occured during stack manipulation since
     *  last reset. */
    bool isError() const;
    /*! Resets error flag. */
    void resetError();

private:
    /*! Maximum number of elements stored in the stack. */
    unsigned maxSize_;
    /*! The index of the top of the stack. */
    unsigned vacant_;
    /*! Stack content. */
    boost::scoped_array<T> data_;
    /*! Error flag. */
    bool error_;
	
    /*! Disabled default copy constructor. */
    SimpleStack(const SimpleStack&);
    /*! Disabled assingment operator. */
    SimpleStack& operator=(const SimpleStack&);
};

template <typename T>
SimpleStack<T>::SimpleStack(unsigned size) :
    maxSize_(size), vacant_(0), data_(new T[maxSize_]), error_(false) {}

template <typename T>
bool SimpleStack<T>::empty() const
{
    return vacant_ == 0;
}

template <typename T>
unsigned SimpleStack<T>::size() const
{
    return vacant_;
}

template <typename T>
unsigned SimpleStack<T>::maxSize() const
{
    return maxSize_;
}

template <typename T>
bool SimpleStack<T>::full() const
{
    return vacant_ == maxSize_;
}

template <typename T>
void SimpleStack<T>::pop()
{
    if (vacant_ > 0) vacant_--;
}

template <typename T>
void SimpleStack<T>::push(const T& value)
{
    if (!this->full()) {
	data_[vacant_] = value;
	vacant_++;
    } else {
	error_ = true;
    }
}

template <typename T>
T& SimpleStack<T>::top()
{
    if (!this->empty()) {
	return data_[vacant_ - 1];
    } else {
	error_ = true;
	return data_[0];
    }
}

template <typename T>
bool SimpleStack<T>::isError() const
{
    return error_;
}

template <typename T>
void SimpleStack<T>::resetError()
{
    error_ = false;
}
    
} /* namespace vartrace */

#endif /* SIMPLESTACK_H */
