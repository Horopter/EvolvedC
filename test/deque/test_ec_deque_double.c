#include "../../src/ec_deque.h"
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
test_deque_double_sequence(void)
{
	stamp();
	printf("test_deque_double_sequence\n");
	ec_deque_dbl_t d;
	assert(ec_deque_dbl_init(&d, NULL) == EC_OK);

	double x = 1.5, y = 2.5, z = 3.5;
	assert(d.push_front(&d, &x) == EC_OK);
	assert(d.push_front(&d, &y) == EC_OK);
	assert(d.push_back(&d, &z) == EC_OK);
	assert(d.size_fn(&d) == 3);

	void* pv = NULL;
	double* dv = NULL;
	assert(d.pop_back(&d, &pv) == EC_OK);
	dv = (double*)pv;
	assert(*dv == 3.5);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_OK);
	dv = (double*)pv;
	assert(*dv == 2.5);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_OK);
	dv = (double*)pv;
	assert(*dv == 1.5);
	free(pv);
	assert(d.size_fn(&d) == 0);

	assert(d.free_fn(&d) == EC_OK);
	printf("pass:test_deque_double_sequence\n");
}

int
main(void)
{
	printf("Starting ec_deque_double tests...\n");
	test_deque_double_sequence();
	printf("All ec_deque_double tests passed.\n");
	return 0;
}
