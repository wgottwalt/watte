/*
 *  Watte 2 - weird and trivially tiny editor (C version)
 *  Copyright (C) 2020 Wilken 'Akiko' Gottwalt <akiko@linux-addicted.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <errno.h>
#include "support.h"

ssize_t _zero(void *src, ssize_t size)
{
	if (!src)
		return -EFAULT;

	char *ptr = src;
	ssize_t i;

	for (i = 0; i < size; ++i)
		ptr[i] = 0;

	return i;
}

ssize_t _length(const void *src, size_t size)
{
	if (!src)
		return -EFAULT;

	const char *ptr = src;
	size_t i;

	for (i = 0; ptr[i] != '\0'; ++i)
		if (i >= size)
			return -ENOMEM;

	return i;
}
