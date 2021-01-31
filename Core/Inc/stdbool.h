/* Copyright (C) 2001-2003, 2006-2017 Free Software Foundation, Inc.
   Written by Bruno Haible <haible@clisp.cons.org>, 2001.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef _STDBOOL_H
#define _STDBOOL_H

/* ISO C 99 <stdbool.h> for platforms that lack it.  */

#  define _Bool signed char
#  define bool _Bool

/* The other macros must be usable in preprocessor directives.  */
#ifdef __cplusplus
# define false false
# define true true
#else
# undef false
# define false 0
# undef true
# define true 1
#endif

#define __bool_true_false_are_defined 1

#endif /* _UNISTRING_STDBOOL_H */
