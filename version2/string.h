#pragma once
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

#include <sys/types.h>

#define STRING_MAX_SIZE 4096
#define STRING_DEF_SIZE 128

struct string_t {
	ssize_t length;
	ssize_t capacity;
	char *data;
};

ssize_t string_init(struct string_t *str);
ssize_t string_init_cstr(struct string_t *str, const char *src);
ssize_t string_clear(struct string_t *str);
ssize_t string_copy(struct string_t *dest, const struct string_t *src);
ssize_t string_concat(struct string_t *dest, const struct string_t *src);
ssize_t string_destroy(struct string_t *str);
