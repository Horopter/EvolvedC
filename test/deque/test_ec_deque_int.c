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
test_deque_int_push_pop_peek(void)
{
	stamp();
	printf("test_deque_int_push_pop_peek\n");
	ec_deque_int_t d;
	assert(ec_deque_int_init(&d, NULL) == EC_OK);

	int a = 10, b = 20, c = 30;
	assert(d.push_back(&d, &a) == EC_OK);
	assert(d.push_front(&d, &b) == EC_OK);
	assert(d.push_back(&d, &c) == EC_OK);
	assert(d.size_fn(&d) == 3);

	void* pv = NULL;
	int* iv = NULL;
	assert(d.peek_front(&d, &pv) == EC_OK);
	iv = (int*)pv;
	assert(*iv == 20);
	free(pv);
	assert(d.peek_back(&d, &pv) == EC_OK);
	iv = (int*)pv;
	assert(*iv == 30);
	free(pv);

	/* pop in order: front -> 20, back -> 30, front -> 10 */
	assert(d.pop_front(&d, &pv) == EC_OK);
	iv = (int*)pv;
	assert(*iv == 20);
	free(pv);
	assert(d.pop_back(&d, &pv) == EC_OK);
	iv = (int*)pv;
	assert(*iv == 30);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_OK);
	iv = (int*)pv;
	assert(*iv == 10);
	free(pv);

	/* empty now */
	assert(d.pop_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);

	assert(d.free_fn(&d) == EC_OK);
	printf("pass:test_deque_int_push_pop_peek\n");
}

int
main(void)
{
	printf("Starting ec_deque_int tests...\n");
	test_deque_int_push_pop_peek();
	printf("All ec_deque_int tests passed.\n");
	return 0;
}
