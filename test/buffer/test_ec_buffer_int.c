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
test_buf_init_put_get_peek_compact(void)
{
	stamp();
	printf("test_buf_init_put_get_peek_compact\n");
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 4, NULL));

	/* put grows if needed */
	for (int i = 0; i < 6; i++)
		{
			assert(ec_buf_int_put(&b, i + 1));
		}
	assert(ec_buf_int_size(&b) == 6);

	int v;
	/* peek does not advance */
	assert(ec_buf_int_peek(&b, &v) && v == 1);
	assert(ec_buf_int_size(&b) == 6);

	/* get advances */
	assert(ec_buf_int_get(&b, &v) && v == 1);
	assert(ec_buf_int_get(&b, &v) && v == 2);
	assert(ec_buf_int_size(&b) == 4);

	/* compact shifts unread data to front */
	ec_buf_int_compact(&b);
	assert(ec_buf_int_size(&b) == 4);
	assert(ec_buf_int_peek(&b, &v) && v == 3);

	ec_buf_int_free(&b);
	printf("pass:test_buf_init_put_get_peek_compact\n");
}

static void
test_buf_wrap_and_valid(void)
{
	stamp();
	printf("test_buf_wrap_and_valid\n");
	int data[3] = { 10, 20, 30 };
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 1, NULL));
	ec_buf_int_wrap(&b, data, 3);

	int v;
	assert(ec_buf_int_peek(&b, &v) && v == 10);
	assert(ec_buf_int_get(&b, &v) && v == 10);
	assert(ec_buf_int_get(&b, &v) && v == 20);
	assert(ec_buf_int_get(&b, &v) && v == 30);
	/* Now empty: get should set err */
	assert(!ec_buf_int_get(&b, &v));
	assert(!ec_buf_int_valid(&b));

	ec_buf_int_free(&b);
	printf("pass:test_buf_wrap_and_valid\n");
}

int
main(void)
{
	printf("Starting ec_buffer_int tests...\n");
	test_buf_init_put_get_peek_compact();
	test_buf_wrap_and_valid();
	printf("All ec_buffer_int tests passed.\n");
	return 0;
}
