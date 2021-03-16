/*
 *  Watte - weird and trivially tiny editor
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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STRING_MAX_SIZE 4096
#define STRING_DEF_SIZE 128

/*--- support functions ---*/

void _zero(void *src, size_t size)
{
	char *ptr = src;

	for (size_t i = 0; i < size; ++i)
		ptr[i] = 0;
}

size_t _length(const void *src, size_t size)
{
	const char *ptr = src;
	size_t i;

	for (i = 0; (i < size) || (ptr[i] == '\0'); ++i);

	return i;
}

/*--- string structure and functions ---*/

struct string {
	size_t length;
	size_t capacity;
	char *data;
};

void string_destroy(struct string *str)
{
	if (!str)
		return;

	if (str->data) {
		free(str->data);
		str->length = 0;
		str->capacity = 0;
		str->data = NULL;
	}
}

void string_clear(struct string *str)
{
	if (!str)
		return;

	if (str->data) {
		_zero(str->data, str->capacity);
		str->length = 0;
	}
}

void string_init(struct string *str)
{
	if (!str)
		return;

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
	string_clear(str);

	return;

string_init_fail:
	if (str->data)
		free(str->data);
	str->length = 0;
	str->capacity = 0;
	str->data = NULL;
}

void string_init_from(struct string *str, const char *src)
{
	if (!str || !src)
		return;

	const size_t size = _length(src, STRING_MAX_SIZE);

	if (str->data) {
		char *tmp = (char *)realloc(str->data, size + 1);

		if (!tmp)
			return;

		str->length = size;
		str->capacity = size + 1;
		str->data = tmp;
	} else {
		str->data = (char *)malloc(size + 1);

		if (!str->data) {
			str->length = 0;
			str->capacity = 0;

			return;
		}
	}

	for (size_t i = 0; i < size; ++i)
		str->data[i] = src[i];
	str->data[size] = 0;
}

/*--- stringlist structure and functions ---*/

struct stringlist {
};

/*--- editor structure and functions ---*/

struct editor {
	struct stringlist data;
	struct string filename;
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

int32_t main(int32_t argc, char **argv)
{
	struct editor ed;

	_zero(&ed, sizeof (ed));

	return editor_run(&ed, (argc == 2) ? argv[1] : "noname.txt");
}
