#if !defined __PG_LIBC_H
# define __PG_LIBC_H 20220404L

# include "lib/fmath.h"
# include <String.h>

size_t strlen(const char* s)
{
	std::size_t len = 0;

	while (*s++)
		++len;

	return len;
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
