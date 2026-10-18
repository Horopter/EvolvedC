#include "../../src/ec_deque.h"
#include <assert.h>
#include <stdio.h>

typedef struct
{
	int id;
} rec_t;
EC_DEQUE_DEF(rec_t, rec)

static void
test_push_peek_pos(void)
{
	ec_deque_rec_t d;
	assert(ec_deque_rec_init(&d, NULL) == EC_OK);
	rec_t r1 = { 1 }, r2 = { 2 };
	rec_t* rout;
	assert(d.push_back(&d, &r1) == EC_OK);
	assert(d.push_front(&d, &r2) == EC_OK);
	assert(d.peek_front(&d, (void**)&rout) == EC_OK && rout->id == 2);
	assert(d.peek_back(&d, (void**)&rout) == EC_OK && rout->id == 1);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_pop_order_pos(void)
{
	ec_deque_rec_t d;
	assert(ec_deque_rec_init(&d, NULL) == EC_OK);
	rec_t r1 = { 1 }, r2 = { 2 }, r3 = { 3 };
	rec_t* rout;
	assert(d.push_back(&d, &r1) == EC_OK);
	assert(d.push_front(&d, &r2) == EC_OK);
	assert(d.push_back(&d, &r3) == EC_OK);
	assert(d.pop_front(&d, (void**)&rout) == EC_OK && rout->id == 2);
	assert(d.pop_back(&d, (void**)&rout) == EC_OK && rout->id == 3);
	assert(d.pop_back(&d, (void**)&rout) == EC_OK && rout->id == 1);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_size_pos(void)
{
	ec_deque_rec_t d;
	assert(ec_deque_rec_init(&d, NULL) == EC_OK);
	rec_t r1 = { 1 }, r2 = { 2 }, r3 = { 3 };
	assert(d.size_fn(&d) == 0);
	assert(d.push_back(&d, &r1) == EC_OK);
	assert(d.size_fn(&d) == 1);
	assert(d.push_front(&d, &r2) == EC_OK);
	assert(d.size_fn(&d) == 2);
	assert(d.push_back(&d, &r3) == EC_OK);
	assert(d.size_fn(&d) == 3);
	assert(d.free_fn(&d) == EC_OK);
}

static void
test_empty_and_invalid_neg(void)
{
	ec_deque_rec_t d;
	assert(ec_deque_rec_init(&d, NULL) == EC_OK);
	rec_t* rout;
	assert(d.pop_front(&d, (void**)&rout) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.pop_back(&d, (void**)&rout) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_front(&d, (void**)&rout) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.peek_back(&d, (void**)&rout) == EC_ERR_OUT_OF_BOUNDS);
	assert(d.free_fn(&d) == EC_OK);
	/* invalid */
	assert(ec_deque_rec_init(NULL, NULL) == EC_ERR_INVALID_ARG);
	assert(ec_deque_rec_pop_front(NULL, (void**)&rout) == EC_ERR_INVALID_ARG);
	assert(ec_deque_rec_pop_back(NULL, (void**)&rout) == EC_ERR_INVALID_ARG);
	assert(ec_deque_rec_peek_front(NULL, (void**)&rout) == EC_ERR_INVALID_ARG);
	assert(ec_deque_rec_peek_back(NULL, (void**)&rout) == EC_ERR_INVALID_ARG);
}

int
main(void)
{
	printf("Starting ec_deque_rec tests...\n");
	test_push_peek_pos();
	test_pop_order_pos();
	test_size_pos();
	test_empty_and_invalid_neg();
	printf("All ec_deque_rec tests passed.\n");
	return 0;
}
