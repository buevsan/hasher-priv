
/*
  Copyright (C) 2003-2007  Dmitry V. Levin <ldv@altlinux.org>

  The entry function for the hasher-priv program.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Code in this file may be executed with root privileges. */

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>

#include "priv.h"

static void
my_error_print_progname(void)
{
	fprintf(stderr, "%s: ", program_invocation_short_name);
}

int
main(int ac, const char *av[])
{
	task_t  task;

	error_print_progname = my_error_print_progname;

	/* First, check and sanitize file descriptors. */
	sanitize_fds();

	/* Second, parse command line arguments. */
	task = parse_cmdline(ac, av);

	if (chroot_path && *chroot_path != '/')
		error(EXIT_FAILURE, 0, "%s: invalid chroot path",
		      chroot_path);

	/* Third, initialize data related to caller. */
	init_caller_data();

	/* 4th, parse environment for config options. */
	parse_env();

	/* We don't need environment variables any longer. */
	if (clearenv() != 0)
		error(EXIT_FAILURE, errno, "clearenv");

	/* Load config according to caller information. */
	configure();

	/* Finally, execute choosen task. */
	switch (task)
	{
		case TASK_GETCONF:
			return do_getconf();
		case TASK_KILLUID:
			return do_killuid();
		case TASK_GETUGID1:
			return do_getugid1();
		case TASK_CHROOTUID1:
			return do_chrootuid1();
		case TASK_GETUGID2:
			return do_getugid2();
		case TASK_CHROOTUID2:
			return do_chrootuid2();
		case TASK_MAKEDEV:
			return do_makedev();
		case TASK_MAKETTY:
			return do_maketty();
		case TASK_MAKECONSOLE:
			return do_makeconsole();
		case TASK_MOUNT:
			return do_mount();
		case TASK_UMOUNT:
			return do_umount();
		default:
			error(EXIT_FAILURE, 0, "unknown task %d", task);
	}

	return EXIT_FAILURE;
}
