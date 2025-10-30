#include "../../src/ec_array.h"
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
test_array_uint_bounds(void)
{
	stamp();
	printf("test_array_uint_bounds\n");
	ec_array_uint_t a;
	assert(ec_array_uint_init(&a, 2, NULL) == EC_OK);
	assert(a.push(&a, 1u) == EC_OK);
	assert(a.push(&a, 2u) == EC_OK);
	/* full */
	assert(a.push(&a, 3u) == EC_ERR_MAX_CAPACITY);

	unsigned int v;
	assert(a.get(&a, 1, &v) == EC_OK && v == 2u);
	assert(a.get(&a, 2, &v) == EC_ERR_OUT_OF_BOUNDS);
	assert(a.set(&a, 1, 5u) == EC_OK);
	assert(a.get(&a, 1, &v) == EC_OK && v == 5u);
	assert(a.set(&a, 2, 7u) == EC_ERR_OUT_OF_BOUNDS);

	a.free(&a);
	printf("pass:test_array_uint_bounds\n");
}

int
main(void)
{
	printf("Starting ec_array_uint tests...\n");
	test_array_uint_bounds();
	printf("All ec_array_uint tests passed.\n");
	return 0;
}
