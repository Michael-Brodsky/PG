/*
 *	This file defines several functions in the C Standard Library header 
 *	<string.h> and can be used with implementations that lack one, such as the 
 *	`megaavr' framework.
 *
 *  ***************************************************************************
 *
 *	File: string.h
 *	Date: April 16, 2022 8, 2021
 *	Version: 1.0
 *	Author: Michael Brodsky
 *	Email: mbrodskiis@gmail.com
 *	Copyright (c) 2012-2022 Michael Brodsky
 *
 *	***************************************************************************
 *
 *  This file is part of "Pretty Good" (Pg). "Pg" is free software:
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
 *	**************************************************************************/

#if !defined __PG_LIBC_H
# define __PG_LIBC_H 20220404L

# include "lib/fmath.h"
# include <String.h>

void* memchr(const void* str, int ch, size_t n)
{
	unsigned char* p = (unsigned char*)str;
	unsigned char* result = nullptr;

	while (str && n--)
	{
		if (*p != (unsigned char)ch)
			p++;
		else
		{
			result = p;
			break;
		}
	}

	return result;
}

int memcmp(const void* lhs, const void* rhs, std::size_t count)
{
	unsigned char* p = (unsigned char*)lhs;
	unsigned char* q = (unsigned char*)rhs;
	int result = 0;

	if (lhs != rhs)
	{
		while (count > 0)
		{
			if (*p != *q)
			{
				result = (*p > *q) ? 1 : -1;
				break;
			}
			count--;
			p++;
			q++;
		}
	}

	return result;
}

char* strcat(char* dest, const char* src)
{
	char* ptr = dest + strlen(dest);

	while (*src)
		*ptr++ = *src++;
	*ptr = '\0';

	return dest;
}

//
// Defined in megaavr implementation.
//
//const char* strchr(const char* str, int ch) 
//{
//	for (; *str != '\0' && *str != (const char)ch; ++str);
//	
//	return *str == (const char)ch ? (const char*)str : nullptr;
//}

char* strchr(char* str, int ch)
{
	for (; *str != '\0' && *str != (char)ch; ++str);

	return *str == (char)ch ? (char*)str : nullptr;
}

size_t strlen(const char* s)
{
	std::size_t len = 0;

	while (*s++)
		++len;

	return len;
}

char* strncat(char* dest, const char* src, size_t count)
{
	char* ptr = dest + strlen(dest);

	while (*src && count--)
		*ptr++ = *src++;
	*ptr = '\0';

	return dest;
}

int strncmp(const char* lhs, const char* rhs, std::size_t count)
{
	int result = 0;

	while (count-- && *lhs && *rhs)
	{
		if ((result = *lhs++ - *rhs++))
			break;
	}

	return pg::sign(result);
}

char* strncpy(char* dest, const char* src, std::size_t count)
{
	char* cpy = dest;

	while (count--)
	{
		*dest++ = *src;
		if (!*src++)
			break;
	}

	return cpy;
}

//
// Defined in megaavr implementation.
//
//const char* strrchr(const char* str, int ch)
//{
//	const char* result = nullptr;
//
//	do 
//	{
//		if (*str == (const char)ch)
//			result = (const char*)str;
//	} while (*str++);
//
//	return result;
//}

char* strrchr(char* str, int ch)
{
	char* result = nullptr;

	do
	{
		if (*str == (char)ch)
			result = (char*)str;
	} while (*str++);

	return result;
}

char* strtok(char* s1, const char* s2)
{
	static String input;
	static String result;

	if (s1)
		input = s1;
	if (!input.length())
		return nullptr;
	String d = s2;
	std::size_t len = d.length();
	for (std::size_t i = 0; i < len; ++i)
	{
		auto n = input.indexOf(d[i]);

		if (n)
		{
			if (n > 0)
			{
				result = input.substring(0, n++);
				input = input.substring(n);
			}
			else
			{
				result = input;
				input = "";
			}
			break;
		}
	}

	return const_cast<char*>(result.c_str());
}

#endif
