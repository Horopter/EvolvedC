#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void
test_init_free_pos(void)
{
	ec_deque_str_t d;
	assert(ec_deque_str_init(&d, NULL) == EC_OK);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_push_front_back_pos(void)
{
	ec_deque_str_t d;
	assert(ec_deque_str_init(&d, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	assert(d.push_back(&d, &a) == EC_OK);
	assert(d.push_front(&d, &b) == EC_OK);
	assert(d.push_back(&d, &c) == EC_OK);
	assert(d.size_fn(&d) == 3);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_peek_front_back_pos(void)
{
	ec_deque_str_t d;
	assert(ec_deque_str_init(&d, NULL) == EC_OK);
	const char *a = "a", *b = "b";
	void* pv = NULL;
	const char** sp;
	assert(d.push_back(&d, &a) == EC_OK);
	assert(d.push_front(&d, &b) == EC_OK);
	assert(d.peek_front(&d, &pv) == EC_OK);
	sp = (const char**)pv;
	assert(strcmp(*sp, "b") == 0);
	free(pv);
	assert(d.peek_back(&d, &pv) == EC_OK);
	sp = (const char**)pv;
	assert(strcmp(*sp, "a") == 0);
	free(pv);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_pop_sequence_and_empty_neg(void)
{
	ec_deque_str_t d;
	assert(ec_deque_str_init(&d, NULL) == EC_OK);
	const char *a = "a", *b = "b", *c = "c";
	void* pv = NULL;
	const char** sp;
	assert(d.push_back(&d, &a) == EC_OK);
	assert(d.push_front(&d, &b) == EC_OK);
	assert(d.push_back(&d, &c) == EC_OK);
	assert(d.pop_front(&d, &pv) == EC_OK);
	sp = (const char**)pv;
	assert(strcmp(*sp, "b") == 0);
	free(pv);
	assert(d.pop_back(&d, &pv) == EC_OK);
	sp = (const char**)pv;
	assert(strcmp(*sp, "c") == 0);
	free(pv);
	assert(d.pop_back(&d, &pv) == EC_OK);
	sp = (const char**)pv;
	assert(strcmp(*sp, "a") == 0);
	free(pv);
	assert(d.pop_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.pop_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_front(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_back(&d, &pv) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_invalid_args_neg(void)
{
	ec_deque_str_t d;
	assert(ec_deque_str_init(&d, NULL) == EC_OK);
	void* pv = NULL;
	assert(d.pop_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.pop_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_front(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.peek_back(NULL, &pv) == EC_ERR_INVALID_ARG);
	assert(d.free_fn(&d) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_deque_str tests...\n");
	test_init_free_pos();
	test_push_front_back_pos();
	test_peek_front_back_pos();
	test_pop_sequence_and_empty_neg();
	test_invalid_args_neg();
	printf("All ec_deque_str tests passed.\n");
	return 0;
}
