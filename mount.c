
/*
  $Id$
  Copyright (C) 2004  Dmitry V. Levin <ldv@altlinux.org>

  The mount action for the hasher-priv program.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <errno.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <grp.h>
#include <sys/mount.h>

#include "priv.h"
#include "xmalloc.h"

static void
xmount (const char *fstype, const char *grname)
{
	struct group *gr;
	char   *options = 0;

	if (grname && (gr = getgrnam (grname)))
	{
		xasprintf (&options, "gid=%u", gr->gr_gid);
		endgrent ();
	}

	if (mount (fstype, ".", fstype, MS_MGC_VAL, options ? : "") < 0)
		error (EXIT_FAILURE, errno, "mount: %s", fstype);

	free (options);
}

static void
mount_proc (void)
{
	chdiruid (chroot_path);
	chdiruid ("proc");
	xmount ("proc", "proc");
}

static void
mount_devpts (void)
{
	chdiruid (chroot_path);
	chdiruid ("dev");
	chdiruid ("pts");
	xmount ("devpts", "tty");
}

static void
mount_sysfs (void)
{
	chdiruid (chroot_path);
	chdiruid ("sys");
	xmount ("sysfs", 0);
}

int
do_mount (void)
{
	char   *fstypes = allowed_fstypes ? xstrdup (allowed_fstypes) : 0;
	char   *fs;

	for (fs = fstypes ? strtok (fstypes, " \t,") : 0; fs;
	     fs = strtok (0, " \t,"))
		if (!strcasecmp (fs, mount_fstype))
			break;

	if (!fs)
		error (EXIT_FAILURE, 0,
		       "mount: %s: file system type not allowed",
		       mount_fstype);

	if (!strcmp (fs, "proc"))
		mount_proc ();
	else if (!strcmp (fs, "devpts"))
		mount_devpts ();
	else if (!strcmp (fs, "sysfs"))
		mount_sysfs ();
	else
		error (EXIT_FAILURE, 0,
		       "mount: %s: file system type not supported", fs);

	free (fstypes);
	return 0;
}
