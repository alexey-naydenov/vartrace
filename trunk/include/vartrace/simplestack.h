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

#include <boost/scoped_array.hpp>

namespace vartrace {

/*! Stores data by value.
 *
 * Tries to follow stl stack interface. */
template <typename T> class SimpleStack
{
public:
    enum { DefaultStackSize = 32 };
    
    SimpleStack(unsigned size);
    virtual ~SimpleStack() {};

    bool empty() const;
    void pop();
    void push(const T& value);
    unsigned size() const;
    T& top();

    bool full() const;
    bool isError() const;
    void resetError();

private:
    unsigned size_;
    unsigned vacant_;
    boost::scoped_array<T> data_;
    bool error_;
	
    /*! Disabled default copy constructor. */
    SimpleStack(const SimpleStack&);
    /*! Disabled assingment operator. */
    SimpleStack& operator=(const SimpleStack&);
};

template <typename T>
SimpleStack<T>::SimpleStack(unsigned size = SimpleStack::DefaultStackSize) :
    size_(size), vacant_(0), data_(new T[size_]), error_(false) {}

template <typename T>
bool SimpleStack<T>::empty() const
{
    return vacant_ == 0;
}

template <typename T>
bool SimpleStack<T>::full() const
{
    return vacant_ == size_;
}

template <typename T>
void SimpleStack<T>::pop()
{
    if (vacant_ > 0) vacant_--;
}

template <typename T>
void SimpleStack<T>::push(const T& value)
{
    if (!this.full()) {
	data_[vacant_] = value;
	vacant_++;
    } else {
	error_ = true;
    }
}

template <typename T>
T& SimpleStack<T>::top()
{
    if (!this.empty()) {
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
