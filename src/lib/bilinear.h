/*
 *	This file defines a library algorithms for computing bilinear transforms.
 *
 *	***************************************************************************
 *
 *	File: bilinear.h
 *	Date: September 14, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky
 *
 *	***************************************************************************
 *
 *  This file is part of `Pretty Good' (Pg). `Pg' is free software:
 *	you can redistribute it and/or modify it under the terms of the
 *	GNU General Public License as published by the Free Software Foundation,
 *	either version 3 of the License, or (at your option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *	WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *	along with this file. If not, see <http://www.gnu.org/licenses/>.
 *
 *	**************************************************************************
 *
 *	Description:
 * 
 *	    This file several algorithms for computing discrete time (digital) 
 *      transfer function coefficients from their continuous-time (analog) 
 *      counterparts using the bilinear transform, where: 
 * 
 *                        |      2   z - 1  
 *          Hd(z) = Ha(s) | s = --- ------- . 
 *                        |      T   z + 1 
 * 
 ******************************************************************************/

#if !defined __PG_BILINEAR_H
# define __PG_BILINEAR_H 20210914L

#include <lib/fmath.h>

# if defined __PG_HAS_NAMESPACES 

namespace pg
{

/* Value for c = 2/T; T = 1/Fs (sample rate), thus c = 2Fs */

#pragma region linear

    template<class T>
    inline  typename details::is_float<T>::type
        bltlinB0(const T& b0, const T& b1, const T& c) { return b0 + b1 * c; }

    template<class T>
    inline  typename details::is_float<T>::type
        bltlinB1(const T& b0, const T& b1, const T& c) { return b0 - b1 * c; }

    template<class T>
    inline  typename details::is_float<T>::type
        bltlinA0(const T& a0, const T& a1, const T& c) { return a0 + a1 * c; }

    template<class T>
    inline  typename details::is_float<T>::type
        bltlinA1(const T& a0, const T& a1, const T& c) { return a0 - a1 * c; }

#pragma endregion
#pragma region quadratic

    template<class T>
    inline  typename details::is_float<T>::type
        bltquadB0(const T& b0, const T& b1, const T& b2, const T& c) 
    { 
        return (b0 + b1 * c + b2 * pg::sqr(c)); 
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltquadB1(const T& b0, const T& b1, const T& b2, const T& c) 
    { 
        return ((b0 + b0) - (b2 + b2) * pg::sqr(c)); 
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltquadB2(const T& b0, const T& b1, const T& b2, const T& c) 
    { 
        return (b0 - b1 * c + b2 * pg::sqr(c)); 
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltquadA0(const T& a0, const T& a1, const T& a2, const T& c) 
    { 
        return (a0 + a1 * c + a2 * pg::sqr(c)); 
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltquadA1(const T& a0, const T& a1, const T& a2, const T& c)
    { 
        return ((a0 + a0) - (a2 + a2) * pg::sqr(c)); 
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltquadA2(const T& a0, const T& a1, const T& a2, const T& c)
    { 
        return (a0 - a1 * c + a2 * pg::sqr(c)); 
    }

#pragma endregion
#pragma region cubic

    template<class T>
    inline  typename details::is_float<T>::type
        bltcubB0(const T& b0, const T& b1, const T& b2, const T& b3, const T& c) 
    {
        return (-b0 - b1 * c - b2 * pg::sqr(c) - b3 * pg::cube(c));
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltcubB1(const T& b0, const T& b1, const T& b2, const T& b3, const T& c)
    {
        return (-(b0 + b0 + b0) - b1 * c + b2 * pg::sqr(c) + (b3 + b3 + b3) * pg::cube(c));
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltcubB2(const T& b0, const T& b1, const T& b2, const T& b3, const T& c)
    {
        return (-(b0 + b0 + b0) + b1 * c + b2 * pg::sqr(c) - (b3 + b3 + b3) * pg::cube(c));
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltcubB3(const T& b0, const T& b1, const T& b2, const T& b3, const T& c)
    {
        return (-b0 + b1 * c - b2 * pg::sqr(c) + b3 * pg::cube(c));
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltcubA0(const T& a0, const T& a1, const T& a2, const T& a3, const T& c) 
    {
        return (-a0 - a1 * c - a2 * pg::sqr(c) - a3 * pg::cube(c));
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltcubA1(const T& a0, const T& a1, const T& a2, const T& a3, const T& c)
    {
        return (-(a0 + a0 + a0) - a1 * c + a2 * pg::sqr(c) + (a3 + a3 + a3) * pg::cube(c));
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltcubA2(const T& a0, const T& a1, const T& a2, const T& a3, const T& c)
    {
        return (-(a0 + a0 + a0) + a1 * c + a2 * pg::sqr(c) - (a3 + a3 + a3) * pg::cube(c));
    }

    template<class T>
    inline  typename details::is_float<T>::type
        bltcubA3(const T& a0, const T& a1, const T& a2, const T& a3, const T& c)
    {
        return (-a0 + a1 * c - a2 * pg::sqr(c) + a3 * pg::cube(c));
    }

#pragma endregion

} // namespace pg

# else // !defined __PG_HAS_NAMESPACES
#  error Requires C++11 and named namespace support.
# endif // defined __PG_HAS_NAMESPACES 

#endif	/* __PG_BILINEAR_H */