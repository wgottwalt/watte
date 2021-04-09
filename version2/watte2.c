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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include "stringlist.h"
#include "support.h"

struct editor {
	struct stringlist_t *data;
	struct string_t filename;
	int32_t xpos;
	int32_t ypos;
	int32_t sline;
	bool running;
};

ssize_t editor_save(const char *filename, struct stringlist_t *data)
{
	FILE *fd = NULL;
	ssize_t count = 0;
	ssize_t err = 0;

	if (!filename)
		return -ENOENT;

	fd = fopen(filename, "w");
	if (!fd)
		return -EIO;

	err = stringlist_first(data);
	if (err < 0)
		return err;

	while (data) {
		err = fwrite(data->string.data, data->string.length, 1, fd);
		if (err == 0)
			return -EIO;
		count += err;
		data = data->next;
	}

	return count;
}

ssize_t editor_run(struct editor *ed, const char *filename)
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

	if (editor_run(&ed, (argc == 2) ? argv[1] : "noname.txt") >= 0)
		return 0;

	return 1;
}
