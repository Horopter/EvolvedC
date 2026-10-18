#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void
test_basic_ops(void)
{
	ec_deque_uint_t d;
	assert(ec_deque_uint_init(&d, NULL) == EC_OK);
	unsigned int a = 10u, b = 20u, c = 30u;
	assert(d.push_back(&d, &a) == EC_OK);
	assert(d.push_front(&d, &b) == EC_OK);
	assert(d.push_back(&d, &c) == EC_OK);
	assert(d.size_fn(&d) == 3);
	void* pv = NULL;
	unsigned int* up;
	assert(d.peek_front(&d, &pv) == EC_OK);
	up = (unsigned int*)pv;
	assert(*up == 20u);
	free(pv);
	assert(d.peek_back(&d, &pv) == EC_OK);
	up = (unsigned int*)pv;
	assert(*up == 30u);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_OK);
	up = (unsigned int*)pv;
	assert(*up == 20u);
	free(pv);
	assert(d.pop_back(&d, &pv) == EC_OK);
	up = (unsigned int*)pv;
	assert(*up == 30u);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_OK);
	up = (unsigned int*)pv;
	assert(*up == 10u);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.pop_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_invalid_args(void)
{
	ec_deque_uint_t d;
	assert(ec_deque_uint_init(&d, NULL) == EC_OK);
	void** out = NULL; /* NULL output */
	assert(d.pop_front(&d, out) == EC_ERR_INVALID_ARG);
	assert(d.pop_back(&d, out) == EC_ERR_INVALID_ARG);
	assert(d.peek_front(&d, out) == EC_ERR_INVALID_ARG);
	assert(d.peek_back(&d, out) == EC_ERR_INVALID_ARG);
	assert(d.free_fn(&d) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_deque_uint tests...\n");
	test_basic_ops();
	test_invalid_args();
	printf("All ec_deque_uint tests passed.\n");
	return 0;
}
