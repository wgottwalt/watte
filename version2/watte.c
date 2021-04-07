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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define STRING_MAX_SIZE 4096
#define STRING_DEF_SIZE 128

/*--- support functions ---*/

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

/*--- string structure and functions ---*/

struct string_t {
	ssize_t length;
	ssize_t capacity;
	char *data;
};

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

/*--- stringlist structure and functions ---*/

struct stringlist_t {
	struct string_t string;
	struct stringlist_t *prev;
	struct stringlist_t *next;
};

ssize_t stringlist_length(struct stringlist_t *list)
{
	if (!list)
		return -EFAULT;

	ssize_t count = 0;
	struct stringlist_t *current = list;

	while (current) {
		current = current->prev;
		++count;
	}
	current = list->next;
	while (current) {
		current = current->next;
		++count;
	}

	return count;
}

ssize_t stringlist_first(struct stringlist_t *list)
{
	if (!list)
		return -EFAULT;

	while (list->prev)
		list = list->prev;

	return 1;
}

ssize_t stringlist_last(struct stringlist_t *list)
{
	if (!list)
		return -EFAULT;

	while (list->next)
		list = list->next;

	return 1;
}

ssize_t stringlist_del_entry(struct stringlist_t *list)
{
	if (!list)
		return -EFAULT;

	struct stringlist_t *current = NULL;

	string_destroy(&list->string);

	if (list->prev) {
		current = list->prev;
		current->next = list->next;
	} else if (list->next) {
		current = list->next;
		current->next = list->next->next;
	}

	free(list);
	list = current;

	return 1;
}

ssize_t stringlist_add_entry(struct stringlist_t *list, const bool before)
{
	if (!list)
		return -EFAULT;

	struct stringlist_t *current = (struct stringlist_t *)malloc(sizeof (struct stringlist_t));
	ssize_t count;

	if (!current)
		return -ENOMEM;

	if (before) {
		current->prev = list->prev;
		current->next = list;
	} else {
		current->prev = list;
		current->next = list->next;
	}
	count = string_init(&current->string);

	if (count) {
		list = current;
		return 1;
	}

	return count;
}

ssize_t stringlist_add_entry_before(struct stringlist_t *list)
{
	return stringlist_add_entry(list, true);
}

size_t stringlist_add_entry_after(struct stringlist_t *list)
{
	return stringlist_add_entry(list, false);
}

ssize_t stringlist_add_entry_from(struct stringlist_t *list, const char *src, const bool before)
{
	ssize_t err = stringlist_add_entry(list, before);

	if (err < 0)
		return err;

	err = string_init_from(&list->string, src);
	if (err < 0)
		stringlist_del_entry(list);

	return err;
}

ssize_t stringlist_add_entry_from_before(struct stringlist_t *list, const char *src)
{
	return stringlist_add_entry_from(list, src, true);
}

size_t stringlist_add_entry_from_after(struct stringlist_t *list, const char *src)
{
	return stringlist_add_entry_from(list, src, false);
}

ssize_t stringlist_destroy(struct stringlist_t *list)
{
	if (!list)
		return -EFAULT;

	ssize_t count = 0;
	struct stringlist_t *current = list;

	stringlist_first(current);

	while (current) {
		stringlist_del_entry(current);
		++count;
	}

	return count;
}

/*--- editor structure and functions ---*/

struct editor {
	struct stringlist_t *data;
	struct string_t filename;
	int32_t xpos;
	int32_t ypos;
	int32_t sline;
	bool running;
};

int32_t editor_run(struct editor *ed, const char *filename)
{
	string_init_from(&ed->filename, filename);

	while (ed->running)
	{
		printf("waiting 10 seconds for now:");
		fflush(stdout);
		for (int32_t i = 9; i >= 0; --i) {
			sleep(1);
			printf(" %d", i);
			fflush(stdout);
		}
		printf("\n");
		ed->running = false;
	}

	string_destroy(&ed->filename);

	return 0;
}

/*--- main ---*/

int32_t main(int32_t argc, char **argv)
{
	struct editor ed;

	_zero(&ed, sizeof (ed));

	return editor_run(&ed, (argc == 2) ? argv[1] : "noname.txt");
}
