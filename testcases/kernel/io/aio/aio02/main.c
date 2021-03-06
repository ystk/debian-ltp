#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "test.h"
#include "usctest.h"
#include "config.h"

#define TEST_NAME "aio_tio"

char *TCID = "aio02/" TEST_NAME;
int TST_TOTAL = 0;

#if HAVE_LIBAIO_H

#include <libaio.h>

int test_main(void);

int main(void)
{
	test_main();
	tst_exit();
}
#else
int main(void)
{
	tst_brkm(TCONF, tst_exit, "libaio missing");
	/* NOTREACHED */
	return 0;
}
#endif
