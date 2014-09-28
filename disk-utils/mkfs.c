/*
 * mkfs		A simple generic frontend for the for the mkfs program
 *		under Linux.  See the manual page for details.
 *
 * Authors:	David Engel, <david@ods.com>
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Ron Sommeling, <sommel@sci.kun.nl>
 *
 * Mon Jul  1 18:52:58 1996: janl@math.uio.no (Nicolai Langfeldt):
 *	Incorporated fix by Jonathan Kamens <jik@annex-1-slip-jik.cam.ov.com>
 * 1999-02-22 Arkadiusz Miśkiewicz <misiek@pld.ORG.PL>
 * - added Native Language Support
 *	
 */

/*
 * This command is deprecated.  The utility is in maintenance mode,
 * meaning we keep them in source tree for backward compatibility
 * only.  Do not waste time making this command better, unless the
 * fix is about security or other very critical issue.
 *
 * See Documentation/deprecated.txt for more information.
 */

#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "c.h"
#include "closestream.h"
#include "nls.h"
#include "xalloc.h"

#ifndef DEFAULT_FSTYPE
#define DEFAULT_FSTYPE	"ext2"
#endif

#define SEARCH_PATH	"PATH=" FS_SEARCH_PATH
#define PROGNAME	"mkfs.%s"


static void __attribute__ ((__noreturn__)) usage(FILE * out)
{
	fprintf(out, _("Usage:\n"));
	fprintf(out, _(" %s [options] [-t <type>] [fs-options] <device> [<size>]\n"),
		     program_invocation_short_name);

	fprintf(out, _("\nOptions:\n"));
	fprintf(out, _(" -t, --type=<type>  filesystem type; when unspecified, ext2 is used\n"));
	fprintf(out, _("     fs-options     parameters for the real filesystem builder\n"));
	fprintf(out, _("     <device>       path to the device to be used\n"));
	fprintf(out, _("     <size>         number of blocks to be used on the device\n"));
	fprintf(out, _(" -V, --verbose      explain what is being done;\n"
		       "                      specifying -V more than once will cause a dry-run\n"));
	fprintf(out, _(" -V, --version      display version information and exit;\n"
		       "                      -V as --version must be the only option\n"));
	fprintf(out, _(" -h, --help         display this help text and exit\n"));

	fprintf(out, USAGE_MAN_TAIL("mkfs(8)"));

	exit(out == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}

static void __attribute__ ((__noreturn__)) print_version(void)
{
	printf(UTIL_LINUX_VERSION);
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	char *progname;		/* name of executable to be called */
	char *fstype = NULL;
	int i, more = 0, verbose = 0;
	char *oldpath, *newpath;

	enum { VERSION_OPTION = CHAR_MAX + 1 };

	static const struct option longopts[] = {
		{"type", required_argument, NULL, 't'},
		{"version", no_argument, NULL, VERSION_OPTION},
		{"verbose", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	atexit(close_stdout);

	if (argc == 2 && !strcmp(argv[1], "-V"))
		print_version();

	/* Check commandline options. */
	opterr = 0;
	while ((more == 0)
	       && ((i = getopt_long(argc, argv, "Vt:h", longopts, NULL))
		   != -1))
		switch (i) {
		case 'V':
			verbose++;
			break;
		case 't':
			fstype = optarg;
			break;
		case 'h':
			usage(stdout);
		case VERSION_OPTION:
			print_version();
		default:
			optind--;
			more = 1;
			break;	/* start of specific arguments */
		}
	if (optind == argc)
		usage(stderr);

	/* If -t wasn't specified, use the default */
	if (fstype == NULL)
		fstype = DEFAULT_FSTYPE;

	/* Set PATH and program name */
	oldpath = getenv("PATH");
	if (!oldpath)
		oldpath = "/bin";

	newpath = xmalloc(strlen(oldpath) + sizeof(SEARCH_PATH) + 3);
	sprintf(newpath, "%s:%s\n", SEARCH_PATH, oldpath);
	putenv(newpath);

	progname = xmalloc(sizeof(PROGNAME) + strlen(fstype) + 1);
	sprintf(progname, PROGNAME, fstype);
	argv[--optind] = progname;

	if (verbose) {
		printf(UTIL_LINUX_VERSION);
		i = optind;
		while (argv[i])
			printf("%s ", argv[i++]);
		printf("\n");
		if (verbose > 1)
			return EXIT_SUCCESS;
	}

	/* Execute the program */
	execvp(progname, argv + optind);
	perror(progname);
	return EXIT_FAILURE;
}
