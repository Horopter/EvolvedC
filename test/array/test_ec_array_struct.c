#include "../../src/ec_array.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct person_a_s
{
	int id;
	char name[16];
} person_a_t;
EC_ARRAY_DEF(person_a_t, person)

static void
test_init_pos(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 2, NULL) == EC_OK);
	a.free(&a);
}

static void
test_init_neg_null(void)
{
	assert(ec_array_person_init(NULL, 2, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_init_neg_zero_capacity(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 0, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_push_pos(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 3, NULL) == EC_OK);
	person_a_t p1 = { 1, "alice" };
	person_a_t p2 = { 2, "bob" };
	person_a_t p3 = { 3, "carol" };
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	assert(a.push(&a, p3) == EC_OK);
	a.free(&a);
}

static void
test_push_capacity_neg(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 2, NULL) == EC_OK);
	person_a_t p1 = { 1, "alice" };
	person_a_t p2 = { 2, "bob" };
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	assert(a.push(&a, p2) == EC_ERR_MAX_CAPACITY);
	a.free(&a);
}

static void
test_push_neg_null(void)
{
	person_a_t p1 = { 1, "alice" };
	assert(ec_array_person_push(NULL, p1) == EC_ERR_INVALID_ARG);
}

static void
test_get_pos(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 2, NULL) == EC_OK);
	person_a_t p1 = { 1, "alice" };
	person_a_t p2 = { 2, "bob" };
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	person_a_t out;
	assert(a.get(&a, 0, &out) == EC_OK && out.id == 1);
	assert(a.get(&a, 1, &out) == EC_OK && out.id == 2);
	a.free(&a);
}

static void
test_get_neg_oob(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 2, NULL) == EC_OK);
	person_a_t p1 = { 1, "alice" };
	assert(a.push(&a, p1) == EC_OK);
	person_a_t out;
	assert(a.get(&a, 2, &out) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_get_neg_null_out(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 2, NULL) == EC_OK);
	person_a_t p1 = { 1, "alice" };
	assert(a.push(&a, p1) == EC_OK);
	assert(a.get(&a, 0, NULL) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_get_neg_null_self(void)
{
	person_a_t out;
	assert(ec_array_person_get(NULL, 0, &out) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_set_pos(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 2, NULL) == EC_OK);
	person_a_t p1 = { 1, "alice" };
	person_a_t p2 = { 2, "bob" };
	person_a_t p3 = { 3, "carol" };
	assert(a.push(&a, p1) == EC_OK);
	assert(a.push(&a, p2) == EC_OK);
	person_a_t out;
	assert(a.set(&a, 1, p3) == EC_OK);
	assert(a.get(&a, 1, &out) == EC_OK && out.id == 3);
	a.free(&a);
}

static void
test_set_neg_oob(void)
{
	ec_array_person_t a;
	assert(ec_array_person_init(&a, 2, NULL) == EC_OK);
	person_a_t p1 = { 1, "alice" };
	assert(a.push(&a, p1) == EC_OK);
	person_a_t p2 = { 2, "bob" };
	assert(a.set(&a, 2, p2) == EC_ERR_OUT_OF_BOUNDS);
	a.free(&a);
}

static void
test_set_neg_null(void)
{
	person_a_t p1 = { 1, "alice" };
	assert(ec_array_person_set(NULL, 0, p1) == EC_ERR_OUT_OF_BOUNDS);
}

static void
test_free_neg_null(void)
{
	ec_array_person_free(NULL); /* Should handle NULL gracefully */
}

int
main(void)
{
	printf("Starting ec_array_person tests...\n");
	test_init_pos();
	test_init_neg_null();
	test_init_neg_zero_capacity();
	test_push_pos();
	test_push_capacity_neg();
	test_push_neg_null();
	test_get_pos();
	test_get_neg_oob();
	test_get_neg_null_out();
	test_get_neg_null_self();
	test_set_pos();
	test_set_neg_oob();
	test_set_neg_null();
	test_free_neg_null();
	printf("All ec_array_person tests passed.\n");
	return 0;
}
