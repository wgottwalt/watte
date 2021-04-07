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
#include "stringlist.h"
#include "support.h"

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

ssize_t stringlist_add_entry_after(struct stringlist_t *list)
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

ssize_t stringlist_add_entry_from_after(struct stringlist_t *list, const char *src)
{
	return stringlist_add_entry_from(list, src, false);
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
