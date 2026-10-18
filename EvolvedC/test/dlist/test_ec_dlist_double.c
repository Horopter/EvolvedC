#include "../../src/ec_dlist.h"
#include <assert.h>
#include <stdio.h>

static void
test_init_free_pos(void)
{
	ec_dlist_dbl_t l;
	assert(ec_dlist_dbl_init(&l, NULL) == EC_OK);
	assert(l.size_fn(&l) == 0);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_add_first_last_pos(void)
{
	ec_dlist_dbl_t l;
	assert(ec_dlist_dbl_init(&l, NULL) == EC_OK);
	double a = 1.5, b = 2.5, c = 3.5;
	assert(l.add_first(&l, &a) == EC_OK);
	assert(l.add_last(&l, &b) == EC_OK);
	assert(l.add_last(&l, &c) == EC_OK);
	assert(l.size_fn(&l) == 3);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_last_pos(void)
{
	ec_dlist_dbl_t l;
	assert(ec_dlist_dbl_init(&l, NULL) == EC_OK);
	double a = 1.5, b = 2.5, c = 3.5;
	assert(l.add_first(&l, &a) == EC_OK);
	assert(l.add_last(&l, &b) == EC_OK);
	assert(l.add_last(&l, &c) == EC_OK);
	void* out;
	assert(l.get_first(&l, &out) == EC_OK && *(double*)out == 1.5);
	assert(l.get_last(&l, &out) == EC_OK && *(double*)out == 3.5);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_last_pos(void)
{
	ec_dlist_dbl_t l;
	assert(ec_dlist_dbl_init(&l, NULL) == EC_OK);
	double a = 1.5, b = 2.5, c = 3.5;
	assert(l.add_first(&l, &a) == EC_OK);
	assert(l.add_last(&l, &b) == EC_OK);
	assert(l.add_last(&l, &c) == EC_OK);
	assert(l.remove_first(&l) == EC_OK);
	assert(l.remove_last(&l) == EC_OK);
	assert(l.size_fn(&l) == 1);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_remove_first_neg_empty(void)
{
	ec_dlist_dbl_t l;
	assert(ec_dlist_dbl_init(&l, NULL) == EC_OK);
	assert(l.remove_first(&l) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_neg_empty(void)
{
	ec_dlist_dbl_t l;
	assert(ec_dlist_dbl_init(&l, NULL) == EC_OK);
	void* out;
	assert(l.get_first(&l, &out) == EC_ERR_OUT_OF_BOUNDS);
	assert(l.free_fn(&l) == EC_OK);
}

static void
test_get_first_neg_null(void)
{
	ec_dlist_dbl_t l;
	assert(ec_dlist_dbl_init(&l, NULL) == EC_OK);
	double a = 1.5;
	assert(l.add_first(&l, &a) == EC_OK);
	void* out;
	assert(l.get_first(NULL, &out) == EC_ERR_INVALID_ARG);
	assert(l.free_fn(&l) == EC_OK);
}

int
main(void)
{
	printf("Starting ec_dlist_double tests...\n");
	test_init_free_pos();
	test_add_first_last_pos();
	test_get_first_last_pos();
	test_remove_first_last_pos();
	test_remove_first_neg_empty();
	test_get_first_neg_empty();
	test_get_first_neg_null();
	printf("All ec_dlist_double tests passed.\n");
	return 0;
}
