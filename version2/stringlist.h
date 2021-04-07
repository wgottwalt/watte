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

#include <stdbool.h>
#include <sys/types.h>

struct stringlist_t {
	struct string_t string;
	struct stringlist_t *prev;
	struct stringlist_t *next;
};

ssize_t stringlist_length(struct stringlist_t *list);
ssize_t stringlist_first(struct stringlist_t *list);
ssize_t stringlist_last(struct stringlist_t *list);
ssize_t stringlist_add_entry(struct stringlist_t *list, const bool before);
ssize_t stringlist_add_entry_before(struct stringlist_t *list);
ssize_t stringlist_add_entry_after(struct stringlist_t *list);
ssize_t stringlist_add_entry_from(struct stringlist_t *list, const char *src, const bool before);
ssize_t stringlist_add_entry_from_before(struct stringlist_t *list, const char *src);
ssize_t stringlist_add_entry_from_after(struct stringlist_t *list, const char *src);
ssize_t stringlist_del_entry(struct stringlist_t *list);
ssize_t stringlist_destroy(struct stringlist_t *list);
