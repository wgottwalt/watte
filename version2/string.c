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
#include <stdlib.h>
#include "string.h"
#include "support.h"

ssize_t string_init(struct string_t *str)
{
	if (!str)
		return -EFAULT;

	if (str->data) {
		if (str->capacity != STRING_DEF_SIZE) {
			char *tmp = (char *)realloc(str->data, STRING_DEF_SIZE);

			if (!tmp)
				goto string_init_fail;
			else
				str->data = tmp;
		}
	} else {
		str->data = (char *)malloc(STRING_DEF_SIZE);
		if (!str->data)
			goto string_init_fail;
	}
	str->length = 0;
	str->capacity = STRING_DEF_SIZE;

	return string_clear(str);

string_init_fail:
	if (str->data)
		free(str->data);
	str->length = 0;
	str->capacity = 0;
	str->data = NULL;

	return -ENOMEM;
}

ssize_t string_init_from(struct string_t *str, const char *src)
{
	if (!str || !src)
		return -EFAULT;

	const ssize_t size = _length(src, STRING_MAX_SIZE);

	if (size < 0)
		return size;

	if (str->data) {
		char *tmp = (char *)realloc(str->data, size + 1);

		if (!tmp)
			return -ENOMEM;

		str->length = size;
		str->capacity = size + 1;
		str->data = tmp;
	} else {
		str->data = (char *)malloc(size + 1);

		if (!str->data) {
			str->length = 0;
			str->capacity = 0;

			return -ENOMEM;
		}
	}

	for (ssize_t i = 0; i < size; ++i)
		str->data[i] = src[i];
	str->data[size] = 0;

	return size;
}

ssize_t string_clear(struct string_t *str)
{
	if (!str)
		return -EFAULT;

	ssize_t count = 0;

	if (str->data) {
		count = _zero(str->data, str->capacity);
		if (count < 0)
			str->capacity = 0;
		str->length = 0;
	}

	return count;
}

ssize_t string_copy(struct string_t *dest, const struct string_t *src)
{
	if (!dest || !src)
		return -EFAULT;

	if ((dest->capacity + 1) <= src->length) {
		char *tmp = (char *)realloc(dest->data, src->length + 1);

		if (!tmp)
			return -ENOMEM;

		dest->data = tmp;
		dest->capacity = src->length + 1;
	}

	dest->length = src->length;

	for (ssize_t i = 0; i < src->length; ++i)
		dest->data[i] = src->data[i];
	for (ssize_t i = dest->length; i < dest->capacity; ++i)
		dest->data[i] = 0;

	return dest->length;
}

ssize_t string_concat(struct string_t *dest, const struct string_t *src)
{
	if (!dest || !src)
		return -EFAULT;

	ssize_t size = dest->length + src->length + 1;

	if ((dest->capacity + 1) <= size) {
		char *tmp = (char *)realloc(dest->data, size);

		if (!tmp)
			return -ENOMEM;

		dest->data = tmp;
		dest->capacity = size;
	}

	dest->length = size - 1;

	for (ssize_t i = 0; i < src->length; ++i)
		dest->data[i + dest->length] = src->data[i];

	return dest->length;
}

ssize_t string_destroy(struct string_t *str)
{
	if (!str)
		return -EFAULT;

	if (str->data) {
		free(str->data);
		str->data = NULL;
	}
	str->length = 0;
	str->capacity = 0;

	return 0;
}
