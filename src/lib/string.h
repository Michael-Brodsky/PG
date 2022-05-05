/*
 *	This file defines several functions in the C Standard Library header 
 *	<string.h> and can be used with implementations that lack one, such as the 
 *	`megaavr' architecture.
 *
 *  ***************************************************************************
 *
 *	File: string.h
 *	Date: May 4, 2022 8, 2021
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

#if !defined __PG_LIBC_STRING_H
# define __PG_LIBC_STRING_H 20220504L

# include "lib/fmath.h" // pg::sign();

# if defined __cplusplus
extern "C" {
# endif

void* memchr(const void* str, int ch, size_t n)
{
	unsigned char* p = (unsigned char*)str;

	while (n--)
	{
		if (*p != (unsigned char)ch)
			p++;
		else
			return p;
	}

	return NULL;
}

int memcmp(const void* lhs, const void* rhs, size_t count)
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

char* strchr(const char* str, int ch)
{
	for (; *str != '\0' && *str != (char)ch; ++str);

	return *str == (char)ch ? (char*)str : NULL;
}

size_t strlen(const char* s)
{
	size_t len = 0;

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

int strncmp(const char* lhs, const char* rhs, size_t count)
{
	int result = 0;

	while (count-- && *lhs && *rhs)
	{
		if ((result = *lhs++ - *rhs++))
			break;
	}

	return pg::sign(result);
}

char* strncpy(char* dest, const char* src, size_t count)
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

char* strpbrk(const char* str, const char* accept)
{
	while (*str != '\0')
	{
		const char* a = accept;

		while (*a != '\0')
		{
			if (*a++ == *str)
				return (char*)str;
		}
		++str;
	}

	return NULL;
}

char* strrchr(const char* str, int ch)
{
	char* result = NULL;

	do
	{
		if (*str == (char)ch)
			result = (char*)str;
	} while (*str++);

	return result;
}

size_t strspn(const char* str, const char* accept)
{
	const char* p;
	const char* a;
	size_t count = 0;

	for (p = str; *p != '\0'; ++p)
	{
		for (a = accept; *a != '\0'; ++a)
			if (*p == *a)
				break;
		if (*a == '\0')
			return count;
		else
			++count;
	}

	return count;
}

char* strstr(const char* string, const char* substring)
{
	const char* a;
	const char* b;

	b = substring;
	if (*b == 0) {
		return (char*)string;
	}
	for (; *string != 0; string += 1) {
		if (*string != *b) {
			continue;
		}
		a = string;
		while (1) {
			if (*b == 0) {
				return (char*)string;
			}
			if (*a++ != *b++) {
				break;
			}
		}
		b = substring;
	}

	return NULL;
}

char* strtok(char* str, const char* delim)
{
	static char* olds;
	char* token;

	if (str == NULL)
		str = olds;
	str += strspn(str, delim);
	if (*str == '\0')
	{
		olds = str;
		return NULL;
	}
	token = str;
	str = strpbrk(token, delim);
	if (str == NULL)
		olds = (char*)memchr(token, '\0', strlen(token));
	else
	{
		**((char**)&str) = '\0';
		olds = str + 1;
	}

	return token;
}

# if defined __cplusplus
}
# endif

#endif // !defined __PG_LIBC_STRING_H
