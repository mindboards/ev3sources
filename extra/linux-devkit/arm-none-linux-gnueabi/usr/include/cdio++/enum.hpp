/*
    $Id: enum.hpp,v 1.2 2008/03/25 15:59:10 karl Exp $

    Copyright (C) 2005, 2008 Rocky Bernstein <rocky@panix.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/** \file enum.hpp
 *
 *  \brief C++ header for pre- and post-increment operators for 
 *  enumerations defined in libcdio that it makes sense to iterate over.
 */

#define ENUM_ITERATE_FNS(type)			\
  inline					\
  type &operator++(type &t)			\
  {						\
    return t = type(t + 1);			\
  }						\
  inline					\
  type  &operator++(type &t, int)		\
  {						\
    return t = type(t + 1);			\
  }						\
  inline					\
  type &operator--(type &t)			\
  {						\
    return t = type(t - 1);			\
  }						\
  inline					\
  type  &operator--(type &t, int)		\
  {						\
    return t = type(t - 1);			\
  }

ENUM_ITERATE_FNS(cdtext_field_t)
ENUM_ITERATE_FNS(driver_id_t)
