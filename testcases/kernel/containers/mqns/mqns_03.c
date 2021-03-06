/*
* Copyright (c) International Business Machines Corp., 2009
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
* the GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Author: Serge Hallyn <serue@us.ibm.com>
*
* Check ipcns+sb longevity
*
* Mount mqueue fs
* unshare
* In unshared process:
*    Create "/mq1" with mq_open()
*    Mount mqueuefs
*    Check that /mq1 exists
*    Create /dev/mqueue/mq2 through vfs (create(2))
*    Umount /dev/mqueue
*    Remount /dev/mqueue
*    Check that both /mq1 and /mq2 exist

***************************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "mqns.h"

char *TCID = "posixmq_namespace_03";
int TST_TOTAL=1;

int p1[2];
int p2[2];

#define FNAM1 DEV_MQUEUE2 SLASH_MQ1
#define FNAM2 DEV_MQUEUE2 SLASH_MQ2

int check_mqueue(void *vtest)
{
	char buf[30];
	mqd_t mqd;
	int rc;
	struct stat statbuf;

	close(p1[1]);
	close(p2[0]);

	read(p1[0], buf, 3); /* go */

	mqd = syscall(__NR_mq_open, SLASH_MQ1, O_RDWR|O_CREAT|O_EXCL, 0755,
			NULL);
	if (mqd == -1) {
		write(p2[1], "mqfail", 7);
		tst_exit();
	}

	mq_close(mqd);

	rc = mount("mqueue", DEV_MQUEUE2, "mqueue", 0, NULL);
	if (rc == -1) {
		perror("mount");
		write(p2[1], "mount1", 7);
		tst_exit();
	}

	rc = stat(FNAM1, &statbuf);
	if (rc == -1) {
		write(p2[1], "stat1", 6);
		tst_exit();
	}

	rc = creat(FNAM2, 0755);
	if (rc == -1) {
		write(p2[1], "creat", 6);
		tst_exit();
	}

	close(rc);

	rc = umount(DEV_MQUEUE2);
	if (rc == -1) {
		perror("umount");
		write(p2[1], "umount", 7);
		tst_exit();
	}

	rc = mount("mqueue", DEV_MQUEUE2, "mqueue", 0, NULL);
	if (rc == -1) {
		write(p2[1], "mount2", 7);
		tst_exit();
	}

	rc = stat(FNAM1, &statbuf);
	if (rc == -1) {
		write(p2[1], "stat2", 7);
		tst_exit();
	}

	rc = stat(FNAM2, &statbuf);
	if (rc == -1) {
		write(p2[1], "stat3", 7);
		tst_exit();
	}

	write(p2[1], "done", 5);

	tst_exit();
}


int main(int argc, char *argv[])
{
	int r;
	char buf[30];
	int use_clone = T_UNSHARE;

	if (argc == 2 && strcmp(argv[1], "-clone") == 0) {
		tst_resm(TINFO, "Testing posix mq namespaces through clone(2).\n");
		use_clone = T_CLONE;
	} else
		tst_resm(TINFO, "Testing posix mq namespaces through unshare(2).\n");

	if (pipe(p1) == -1) { perror("pipe"); exit(EXIT_FAILURE); }
	if (pipe(p2) == -1) { perror("pipe"); exit(EXIT_FAILURE); }

	/* fire off the test */
	r = do_clone_unshare_test(use_clone, CLONE_NEWIPC, check_mqueue, NULL);
	if (r < 0) {
		tst_resm(TFAIL, "failed clone/unshare\n");
		tst_exit();
	}

	tst_resm(TINFO, "Checking correct umount+remount of mqueuefs\n");

	mkdir(DEV_MQUEUE2, 0755);

	close(p1[0]);
	close(p1[0]);
	close(p2[1]);
	write(p1[1], "go", 3);

	read(p2[0], buf, 7);
	r = TFAIL;
	if (!strcmp(buf, "mqfail")) {
		tst_resm(TFAIL, "child process could not create mqueue\n");
		goto fail;
	} else if (!strcmp(buf, "mount1")) {
		tst_resm(TFAIL, "child process could not mount mqueue\n");
		goto fail;
	} else if (!strcmp(buf, "stat1x")) {
		tst_resm(TFAIL, "mq created by child is not in mqueuefs\n");
		goto fail;
	} else if (!strcmp(buf, "creat")) {
		tst_resm(TFAIL, "child couldn't creat mq through mqueuefs\n");
		goto fail;
	} else if (!strcmp(buf, "umount")) {
		tst_resm(TFAIL, "child couldn't umount mqueuefs\n");
		goto fail;
	} else if (!strcmp(buf, "mount2")) {
		tst_resm(TFAIL, "child couldn't remount mqueuefs\n");
		goto fail;
	} else if (!strcmp(buf, "stat2")) {
		tst_resm(TFAIL, "mq_open()d file gone after remount of mqueuefs\n");
		goto fail;
	} else if (!strcmp(buf, "stat3")) {
		tst_resm(TFAIL, "creat(2)'d file gone after remount of mqueuefs\n");
		goto fail;
	}

	tst_resm(TPASS, "umount+remount of mqueuefs remounted the right fs\n");

	r = 0;
fail:
	umount(DEV_MQUEUE2);
	rmdir(DEV_MQUEUE2);
	tst_exit();
}
