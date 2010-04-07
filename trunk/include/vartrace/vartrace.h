/* vartrace.h
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

/*! \file vartrace.h 
 * Template for a variable trace object. 
 */

#ifndef VARTRACE_H
#define VARTRACE_H

#include <boost/scoped_array.hpp>

#include "tracetypes.h"
#include "simplestack.h"

namespace vartrace {

    /*! Class for variable trace objects. */
    class VarTrace
    {
    public:
	enum { MaxMessageDepth = 20 };
	
	/*! Create a trace of given size. */
	VarTrace(size_t size);
	/*! Destructor. */
	virtual ~VarTrace() {};
    private:
	/*! Length of the data array. */
	unsigned length_;
	/*! Pointer to the memory block that contains the trace. */
	boost::scoped_array<AlingmentType> data_;
	/*! Positions of heads for recursive messages. */
	SimpleStack<unsigned> heads_;
	
	/*! Disabled default copy constructor. */
	VarTrace(const VarTrace&);
	/*! Disabled default assingment operator. */
	VarTrace& operator=(const VarTrace&);
    };

}

#endif /* VARTRACE_H */
