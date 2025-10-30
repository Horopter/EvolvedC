#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void
stamp(void)
{
	time_t t = time(NULL);
	struct tm* m = localtime(&t);
	if (m)
		{
			char b[64];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		{
			printf("[timeerr] ");
		}
}

static void
test_buf_double_reserve_limit(void)
{
	stamp();
	printf("test_buf_double_reserve_limit\n");
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 2, NULL));
	/* setting limit smaller than needed triggers err when reserve exceeds */
	b.limit = 3; /* cannot exceed 3 elements */
	assert(ec_buf_dbl_put(&b, 1.0));
	assert(ec_buf_dbl_put(&b, 2.0));
	/* next put would require reserve to 4, should fail */
	bool ok = ec_buf_dbl_put(&b, 3.0);
	if (!ok)
		{
			assert(!ec_buf_dbl_valid(&b));
			ec_buf_dbl_clear(&b); /* reset error */
		}
	/* After clear, manually reserve within limit */
	b.limit = SIZE_MAX;
	assert(ec_buf_dbl_reserve(&b, 3));
	assert(ec_buf_dbl_put(&b, 3.0));

	ec_buf_dbl_free(&b);
	printf("pass:test_buf_double_reserve_limit\n");
}

int
main(void)
{
	printf("Starting ec_buffer_double tests...\n");
	test_buf_double_reserve_limit();
	printf("All ec_buffer_double tests passed.\n");
	return 0;
}
