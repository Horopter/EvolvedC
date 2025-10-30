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
test_array_double_ops(void)
{
	stamp();
	printf("test_array_double_ops\n");
	ec_array_double_t a;
	assert(ec_array_double_init(&a, 4, NULL) == EC_OK);
	assert(a.push(&a, 3.14) == EC_OK);
	assert(a.push(&a, -2.5) == EC_OK);
	double v;
	assert(a.get(&a, 0, &v) == EC_OK && v == 3.14);
	assert(a.set(&a, 1, 1.25) == EC_OK);
	assert(a.get(&a, 1, &v) == EC_OK && v == 1.25);
	a.free(&a);
	printf("pass:test_array_double_ops\n");
}

int
main(void)
{
	printf("Starting ec_array_double tests...\n");
	test_array_double_ops();
	printf("All ec_array_double tests passed.\n");
	return 0;
}
