/*
 *
 *   Copyright (c) International Business Machines  Corp., 2002
 *   Copyright (c) Cyril Hrubis chrubis@suse.cz 2009
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

/*
 * NAME
 *	ftest06.c -- test inode things (ported from SPIE section2/filesuite/ftest7.c, by Airong Zhang)
 *
 * 	this is the same as ftest2, except that it uses lseek64
 *
 * CALLS
 *	open, close,  read, write, llseek,
 *	unlink, chdir
 *
 *
 * ALGORITHM
 *
 *	This was tino.c by rbk.  Moved to test suites by dale.
 *
 *	ftest06 [-f tmpdirname] nchild iterations [partition]
 *
 *	This forks some child processes, they do some random operations
 *	which use lots of directory operations.
 *
 * RESTRICTIONS
 *	Runs a long time with default args - can take others on input
 *	line.  Use with "term mode".
 *	If run on vax the ftruncate will not be random - will always go to
 *	start of file.  NOTE: produces a very high load average!!
 *
 */

#define  _LARGEFILE64_SOURCE 1
#define _GNU_SOURCE
#include <stdio.h>		/* needed by testhead.h		*/
#include <sys/types.h>
#include <sys/param.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include "test.h"
#include "usctest.h"
#include "libftest.h"

char *TCID = "ftest06";
int TST_TOTAL = 1;

#define PASSED 1
#define FAILED 0

static void crfile(int, int);
static void unlfile(int, int);
static void fussdir(int, int);
static void dotest(int, int);
static void dowarn(int, char*, char*);
static void term(int sig);
static void cleanup(void);

#define MAXCHILD	25
#define K_1		1024
#define K_2		2048
#define K_4		4096

static int local_flag;

#define M       (1024*1024)

static int iterations;
static int nchild;
static int parent_pid;
static int pidlist[MAXCHILD];

static char* homedir;
static char* dirname;
static int dirlen;
static char *cwd;
static char *fstyp;

#define SUCCEED_OR_DIE(syscall, message, ...)														\
	(errno = 0,																														\
		({int ret=syscall(__VA_ARGS__);																			\
			if(ret==-1)																												\
				tst_brkm(TBROK, cleanup, message, __VA_ARGS__, strerror(errno)); \
			ret;}))

int main(int ac, char *av[])
{
	int child, status, count, k, j;
	char name[3];

        int lc;
        char *msg;

        /*
         * parse standard options
         */
        if ((msg = parse_opts(ac, av, NULL, NULL)) != NULL){
                tst_resm(TBROK, "OPTION PARSING ERROR - %s", msg);
                tst_exit();
        }

	/*
	 * Default values for run conditions.
	 */
	iterations = 50;
	nchild = 5;

	if (signal(SIGTERM, term) == SIG_ERR) {
		tst_resm(TBROK,"first signal failed");
		tst_exit();
	}

	tst_tmpdir();

	/* use the default values for run conditions */

	cwd = get_current_dir_name();
	asprintf(&dirname, "%s/ftest06", cwd);
	dirlen = strlen(dirname);
	asprintf(&homedir, "%s/ftest06h", cwd);

	SUCCEED_OR_DIE(mkdir, "mkdir(%s, %d) failed: %s", dirname, 0755);
	SUCCEED_OR_DIE(mkdir, "mkdir(%s, %d) failed: %s", homedir, 0755);
	SUCCEED_OR_DIE(chdir, "chdir(%s) failed: %s", dirname);
	SUCCEED_OR_DIE(chdir, "chdir(%s) failed: %s", homedir);
	SUCCEED_OR_DIE(chdir, "chdir(%s) failed: %s", dirname);

	for (lc = 0; TEST_LOOPING(lc); lc++) {

		local_flag = PASSED;
		parent_pid = getpid();

		/* enter block */
		for (k = 0; k < nchild; k++) {
			if ((child = fork()) == 0) {
				dotest(k, iterations);
				tst_exit();
			}
			if (child < 0) {
				tst_resm(TINFO, "System resource may be too low, fork() malloc()"
				                     " etc are likely to fail.");
				tst_brkm(TBROK, cleanup, "Test broken due to inability of fork.");
			}
			pidlist[k] = child;
		}

		/*
		 * Wait for children to finish.
		 */
		count = 0;
		while (errno = 0, (child = wait(&status)) != -1 || errno == EINTR) {
			if(child == -1) continue;
			//tst_resm(TINFO,"Test{%d} exited status = 0x%x", child, status);
			//fprintf(stdout, "status is %d",status);
			if (status) {
				tst_resm(TFAIL,"Test{%d} failed, expected 0 exit.", child);
				local_flag = FAILED;
			}
			++count;
		}

		/*
		 * Should have collected all children.
		 */
		if (count != nchild) {
			tst_resm(TFAIL,"Wrong # children waited on, count = %d", count);
			local_flag = FAILED;
		}

		if (local_flag == PASSED)
			tst_resm(TPASS, "Test passed.");
		else
			tst_resm(TFAIL, "Test failed.");

		if (iterations > 26)
			iterations = 26;

		for (k = 0; k < nchild; k++)
			for (j = 0; j < iterations + 1; j++) {
				ft_mkname(name, dirname, k, j);
				rmdir(name);
				unlink(name);
			}

		SUCCEED_OR_DIE(chdir, "chdir(%s) failed: %s", cwd);
		sync();				/* safeness */

	}

	if (local_flag == FAILED)
                tst_resm(TFAIL, "Test failed.");
        else
                tst_resm(TPASS, "Test passed.");

	cleanup();
	tst_exit();
}

#define	warn(val,m1,m2)	if ((val) < 0) dowarn(me,m1,m2)

/*
 * crfile()
 *	Create a file and write something into it.
 */
static char crmsg[] = "Gee, let's write something in the file!\n";

static void crfile(int me, int count)
{
	int	fd;
	off64_t seekval;
	int	val;
	char	fname[128];
	char	buf[128];

	ft_mkname(fname, dirname, me, count);

	fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (fd < 0 && errno == EISDIR) {
		val = rmdir(fname);
		warn(val, "rmdir", fname);
		fd = open(fname, O_RDWR|O_CREAT|O_TRUNC, 0666);
	}
	warn(fd, "creating", fname);

	seekval = lseek64(fd, (off64_t)(rand() % M), 0);
	warn(seekval, "lseek64", 0);

	val = write(fd, crmsg, sizeof(crmsg)-1);
	warn(val, "write", 0);

	seekval = lseek(fd, -((off64_t)sizeof(crmsg)-1), 1);
	warn(seekval, "lseek64", 0);

	val = read(fd, buf, sizeof(crmsg)-1);
	warn(val, "read", 0);

	if (strncmp(crmsg, buf, sizeof(crmsg)-1))
		dowarn(me, "compare", 0);

	val = close(fd);
	warn(val, "close", 0);
}

/*
 * unlfile()
 *	Unlink some of the files.
 */
static void unlfile(int me, int count)
{
	int val, i;
	char fname[128];

	i = count - 10;
	if (i < 0)
		i = 0;
	for (; i < count; i++) {
		ft_mkname(fname, dirname, me, i);
		val = rmdir(fname);
		if (val < 0 )
			val = unlink(fname);
		if (val == 0 || errno == ENOENT)
			continue;
		dowarn(me, "unlink", fname);
	}
}

/*
 * fussdir()
 *	Make a directory, put stuff in it, remove it, and remove directory.
 *
 * Randomly leave the directory there.
 */
static void fussdir(int me, int count)
{
	int val;
	char dir[128], fname[128], *savedir;

	ft_mkname(dir, dirname, me, count);
	rmdir(dir);
	unlink(dir);

	val = mkdir(dir, 0755);
	warn(val, "mkdir", dir);

	/*
	 * Arrange to create files in the directory.
	 */

	savedir = dirname;
	dirname = "";

	val = chdir(dir);
	warn(val, "chdir", dir);

	crfile(me, count);
	crfile(me, count+1);

	val = chdir("..");
	warn(val, "chdir", "..");

	val = rmdir(dir);

	if (val >= 0) {
		tst_resm(TFAIL,"Test[%d]: rmdir of non-empty %s succeeds!", me, dir);
		tst_exit();
	}

	val = chdir(dir);
	warn(val, "chdir", dir);

	ft_mkname(fname, dirname, me, count);
	val = unlink(fname);
	warn(val, "unlink", fname);

	ft_mkname(fname, dirname, me, count+1);
	val = unlink(fname);
	warn(val, "unlink", fname);

	val = chdir(homedir);
	warn(val, "chdir", homedir);

	if (rand() & 0x01) {
		val = rmdir(dir);
		warn(val, "rmdir", dir);
	}

	dirname = savedir;
}


/*
 * dotest()
 *	Children execute this.
 *
 * Randomly do an inode thing; loop for # iterations.
 */
#define	THING(p)	{p, "p"}

struct	ino_thing {
	void	(*it_proc)();
	char	*it_name;
}	ino_thing[] = {
	THING(crfile),
	THING(unlfile),
	THING(fussdir),
	THING(sync),
};

#define	NTHING	(sizeof(ino_thing) / sizeof(ino_thing[0]))

int	thing_cnt[NTHING];
int	thing_last[NTHING];

static void dotest(int me, int count)
{
	int thing, i;

	//tst_resm(TINFO,"Test %d pid %d starting.", me, getpid());

	srand(getpid());

	for(i = 0; i < count; i++) {
		thing = (rand() >> 3) % NTHING;
		(*ino_thing[thing].it_proc)(me, i, ino_thing[thing].it_name);
		++thing_cnt[thing];
	}

	//tst_resm(TINFO,"Test %d pid %d exiting.", me, getpid());
}


static void dowarn(int me, char *m1, char *m2)
{
	int err = errno;

	tst_resm(TFAIL,"Test[%d]: error %d on %s %s",
		me, err, m1, (m2 ? m2 : ""));
	tst_exit();
}

static void term(int sig LTP_ATTRIBUTE_UNUSED)
{
	int i, status;

	tst_resm(TINFO, "\tterm -[%d]- got sig term.", getpid());

	if (parent_pid == getpid()) {
		for (i=0; i < nchild; i++)
			if (pidlist[i] > 0 && waitpid(pidlist[i], &status, WNOHANG) == 0)		/* avoid embarassment */
				kill(pidlist[i], SIGTERM);
		cleanup();
	}

	tst_resm(TBROK, "Term: Child process exiting.");
	tst_exit();
}

static void cleanup(void)
{
	tst_rmdir();
	tst_exit();
}
