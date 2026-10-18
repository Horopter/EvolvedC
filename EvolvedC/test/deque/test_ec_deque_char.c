#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

EC_DEQUE_DEF(char, ch)

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
		printf("[timeerr] ");
}

static void
test_init_free_pos(void)
{
	ec_deque_ch_t d;
	assert(ec_deque_ch_init(&d, NULL) == EC_OK);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_push_front_back_pos(void)
{
	ec_deque_ch_t d;
	assert(ec_deque_ch_init(&d, NULL) == EC_OK);
	char a = 'a', b = 'b', z = 'z';
	assert(d.push_back(&d, &a) == EC_OK);
	assert(d.push_back(&d, &b) == EC_OK);
	assert(d.push_front(&d, &z) == EC_OK);
	assert(d.size_fn(&d) == 3);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_peek_pop_sequence(void)
{
	ec_deque_ch_t d;
	assert(ec_deque_ch_init(&d, NULL) == EC_OK);
	char a = 'a', b = 'b', z = 'z';
	assert(d.push_back(&d, &a) == EC_OK);
	assert(d.push_back(&d, &b) == EC_OK);
	assert(d.push_front(&d, &z) == EC_OK);
	void* pv = NULL;
	char outc;
	assert(d.pop_front(&d, &pv) == EC_OK);
	outc = *(char*)pv;
	free(pv);
	assert(outc == 'z');
	assert(d.pop_back(&d, &pv) == EC_OK);
	outc = *(char*)pv;
	free(pv);
	assert(outc == 'b');
	assert(d.pop_front(&d, &pv) == EC_OK);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_underflow_neg(void)
{
	ec_deque_ch_t d;
	assert(ec_deque_ch_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(d.pop_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.pop_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_invalid_args_neg(void)
{
	ec_deque_ch_t d;
	assert(ec_deque_ch_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(ec_deque_ch_init(NULL, NULL) == EC_ERR_INVALID_ARG);
	assert(d.pop_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.pop_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.free_fn(&d) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_deque_char tests...\n");
	test_init_free_pos();
	test_push_front_back_pos();
	test_peek_pop_sequence();
	test_underflow_neg();
	test_invalid_args_neg();
	printf("All ec_deque_char tests passed.\n");
	return 0;
}
