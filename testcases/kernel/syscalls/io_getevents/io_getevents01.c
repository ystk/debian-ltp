/*
 *
 *   Copyright (c) Crackerjack Project., 2007
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

/* Porting from Crackerjack to LTP is done
   by Masatake YAMATO <yamato@redhat.com> */

#include "config.h"
#include "test.h"
#include "usctest.h"

char *TCID = "io_getevents01";	/* Test program identifier.    */
extern int Tst_count;		/* Test Case counter for tst_* routines */

int TST_TOTAL = 1;

#ifdef HAVE_LIBAIO_H
#include <libaio.h>
#include <errno.h>
#include <string.h>

/*
 * cleanup()
 * 	performs all the ONE TIME cleanup for this test at completion or
 * 	premature exit
 */
void cleanup(void)
{
	/*
	 * print timing status if that option was specified
	 * print errno log if that option was specified
	 */
	TEST_CLEANUP;

	tst_exit();
}

/*
 * setup() - performs all ONE TIME setup for this test.
 */
void setup()
{
	/* capture signals */
	tst_sig(NOFORK, DEF_HANDLER, cleanup);

	/* Pause if that option was specified */
	TEST_PAUSE;

}				/* End setup() */

int main(int argc, char **argv)
{
	int lc;			/* loop counter */
	char *msg;		/* parse_opts() return message */

	io_context_t ctx;
	long expected_return;

	if ((msg =
	     parse_opts(argc, argv, (option_t *) NULL, NULL)) != (char *)NULL) {
		tst_brkm(TBROK, tst_exit, "OPTION PARSING ERROR - %s", msg);
	 /*NOTREACHED*/}

	setup();

	/* Check for looping state if -i option is given */
	for (lc = 0; TEST_LOOPING(lc); lc++) {
		/* reset Tst_count in case we are looping */
		Tst_count = 0;

		/*
		   DESCRIPTION
		   io_getevents  attempts  to  read  at  least min_nr events and up to nr
		   events from the completion queue  of  the  AIO  context  specified  by
		   ctx_id.   timeout  specifies  the  amount  of time to wait for events,
		   where a NULL timeout waits until at  least  min_nr  events  have  been
		   seen.   Note  that timeout is relative and will be updated if not NULL
		   and the operation blocks.

		   RETURN VALUE
		   io_getevents returns the number of events read: 0  if  no  events  are
		   available or < min_nr if the timeout has elapsed.

		   ERRORS
		   EINVAL ctx_id  is  invalid.  min_nr  is  out  of range or nr is out of
		   range.
		 */
		expected_return = -EINVAL;
		TEST(io_getevents(ctx, 0, 0, NULL, NULL));

		if (TEST_RETURN == 0) {
			tst_resm(TFAIL, "call succeeded unexpectedly");
			continue;
		}

		if (TEST_RETURN == expected_return) {
			tst_resm(TPASS, "expected failure - "
				 "returned value = %ld : %s", TEST_RETURN,
				 strerror(-1 * TEST_RETURN));
		} else {
			tst_resm(TFAIL, "unexpected returned value - %ld - "
				 "expected %ld", TEST_RETURN, expected_return);
		}
		/*
		   EFAULT Either events or timeout is an invalid pointer.

		   ENOSYS io_getevents is not implemented on this architecture.
		 */
		/* Crackerjack has a test case for ENOSYS. But Testing for ENOSYS
		   is not meaningful for LTP, I think.
		   -- Masatake */
	}
	cleanup();

	return 0;
}
#else
int main(int argc, char **argv)
{
	tst_resm(TCONF, "System doesn't support execution of the test");
	return 0;
}
#endif
