#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../../src/ec_vector.h"

/* Instantiate char vector locally */
EC_VECTOR_DEF(char, ch)

static void
print_timestamp(void)
{
	time_t now = time(NULL);
	struct tm* local = localtime(&now);
	char buffer[64];
	if (local)
		{
			strftime(buffer, sizeof(buffer), "[%Y-%m-%d %H:%M:%S]", local);
			printf("%s ", buffer);
		}
	else
		{
			printf("[timestamp error] ");
		}
}

static int
cmp_char(const void* a, const void* b)
{
	char ca = *(const char*)a;
	char cb = *(const char*)b;
	return (int)ca - (int)cb;
}

static void
inc_char(char* c)
{
	*c = (char)(*c + 1);
}

static void
sum_char(char* acc, char* elem, char* out)
{
	*out = (char)((int)(*acc) + (int)(*elem));
}

void
test_vector_char_push_get(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_char_push_get\n");
	ec_vector_ch_t v; assert(ec_vector_ch_init(&v, 4, mptr) == EC_OK);
	assert(v.size == 0);
	assert(v.capacity >= 4);
	assert(v.push(&v, 'a') == EC_OK);
	assert(v.push(&v, 'b') == EC_OK);
	assert(v.push(&v, 'c') == EC_OK);
	char x;
	assert(v.get(&v, 0, &x) == EC_OK && x == 'a');
	assert(v.get(&v, 1, &x) == EC_OK && x == 'b');
	assert(v.get(&v, 2, &x) == EC_OK && x == 'c');
	/* failures */
	assert(v.get(&v, 999, &x) == EC_ERR_OUT_OF_BOUNDS);
	v.free_fn(&v);
	printf("Passed test: vector_char_push_get\n");
}

void
test_vector_char_insert_remove_swap(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_char_insert_remove_swap\n");
	ec_vector_ch_t v; assert(ec_vector_ch_init(&v, 2, mptr) == EC_OK);
	v.push(&v, 'x'); v.push(&v, 'z');
	/* insert 'y' at index 1 -> x y z */
	assert(v.insert(&v, 1, 'y') == EC_OK);
	char x;
	assert(v.get(&v, 1, &x) == EC_OK && x == 'y');
	/* swap 0 and 2 -> z y x */
	assert(v.swap(&v, 0, 2) == EC_OK);
	assert(v.get(&v, 0, &x) == EC_OK && x == 'z');
	/* remove_at 1 -> z x */
	assert(v.remove_at(&v, 1) == EC_OK);
	assert(v.size == 2);
	/* failures */
	assert(v.insert(&v, 999, 'q') == EC_ERR_OUT_OF_BOUNDS);
	assert(v.swap(&v, 0, 999) == EC_ERR_OUT_OF_BOUNDS);
	assert(v.remove_at(&v, 999) == EC_ERR_OUT_OF_BOUNDS);
	v.free_fn(&v);
	printf("Passed test: vector_char_insert_remove_swap\n");
}

void
test_vector_char_contains_index_binary(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_char_contains_index_binary\n");
	ec_vector_ch_t v; assert(ec_vector_ch_init(&v, 8, mptr) == EC_OK);
	const char data[] = { 'd','a','c','b','e' };
	for (size_t i = 0; i < sizeof(data); i++) assert(v.push(&v, data[i]) == EC_OK);
	/* contains/index_of */
	size_t idx = (size_t)-1;
	assert(v.contains(&v, 'c') == 1);
	assert(v.index_of(&v, 'c', &idx) == EC_OK && idx < v.size);
	assert(v.contains(&v, 'z') == 0);
	assert(v.index_of(&v, 'z', &idx) == EC_ERR_NOT_FOUND);
	/* binary_search requires sorted */
	assert(v.sort(&v, cmp_char) == EC_OK);
	char key = 'd';
	assert(v.binary_search(&v, &key, cmp_char, &idx) == EC_OK);
	key = 'q';
	assert(v.binary_search(&v, &key, cmp_char, &idx) == EC_ERR_NOT_FOUND);
	v.free_fn(&v);
	printf("Passed test: vector_char_contains_index_binary\n");
}

void
test_vector_char_map_reduce_filter(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_char_map_reduce_filter\n");
	ec_vector_ch_t v; assert(ec_vector_ch_init(&v, 5, mptr) == EC_OK);
	v.push(&v, 'a'); v.push(&v, 'b'); v.push(&v, 'c');
	/* map: increment each */
	assert(v.map(&v, inc_char) == EC_OK);
	char x; assert(v.get(&v, 0, &x) == EC_OK && x == 'b');
	/* reduce: sum chars */
	char acc = 0, out = 0;
	assert(v.reduce(&v, sum_char, &acc, &out) == EC_OK);
	/* filter: keep > 'b' */
	bool pred(const char c){ return c > 'b'; }
	assert(v.filter_mut(&v, pred) == EC_OK);
	for (size_t i = 0; i < v.size; i++) { assert(v.get(&v, i, &x) == EC_OK && x > 'b'); }
	v.free_fn(&v);
	printf("Passed test: vector_char_map_reduce_filter\n");
}

void
test_vector_char_copy_subarray_trim(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_char_copy_subarray_trim\n");
	ec_vector_ch_t v; assert(ec_vector_ch_init(&v, 6, mptr) == EC_OK);
	v.push(&v, 'a'); v.push(&v, 'b'); v.push(&v, 'c'); v.push(&v, 'd');
	/* subarray 1..3 -> b,c */
	ec_vector_ch_t sub; assert(v.subarray(&v, 1, 3, &sub) == EC_OK);
	assert(sub.size == 2);
	char x; assert(sub.get(&sub, 0, &x) == EC_OK && x == 'b');
	/* shallow/deep copy */
	ec_vector_ch_t cp; assert(v.copy_shallow(&v, &cp) == EC_OK);
	assert(cp.size == v.size);
	/* trim capacity */
	assert(v.trim_capacity(&v) == EC_OK);
	assert(v.capacity == v.size);
	sub.free_fn(&sub);
	cp.free_fn(&cp);
	v.free_fn(&v);
	printf("Passed test: vector_char_copy_subarray_trim\n");
}

void
test_vector_char_iterators(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_char_iterators\n");
	ec_vector_ch_t v; assert(ec_vector_ch_init(&v, 4, mptr) == EC_OK);
	v.push(&v, 'm'); v.push(&v, 'n'); v.push(&v, 'o');
	ec_vector_ch_iter_t it; assert(ec_vector_ch_iter_init(&it, &v) == EC_OK);
	char tmp; size_t cnt = 0; while (ec_vector_ch_iter_next(&it, &tmp) == EC_OK) { cnt++; }
	assert(cnt == v.size);
	/* remove last returned */
	assert(ec_vector_ch_iter_remove(&it) == EC_OK);
	/* add and replace */
	assert(ec_vector_ch_iter_add(&it, 'z') == EC_OK);
	char replaced; assert(ec_vector_ch_iter_replace(&it, 'y', &replaced) == EC_OK);
	v.free_fn(&v);
	printf("Passed test: vector_char_iterators\n");
}

void
test_vector_char_zip_iterators(void* mptr)
{
	print_timestamp();
	printf("Running test: vector_char_zip_iterators\n");
	ec_vector_ch_t a, b; assert(ec_vector_ch_init(&a, 3, mptr) == EC_OK); assert(ec_vector_ch_init(&b, 3, mptr) == EC_OK);
	a.push(&a, 'a'); a.push(&a, 'b'); a.push(&a, 'c');
	b.push(&b, 'x'); b.push(&b, 'y'); b.push(&b, 'z');
	ec_vector_ch_zip_iter_t z; assert(ec_vector_ch_zip_iter_init(&z, &a, &b) == EC_OK);
	char c1, c2; size_t cnt = 0; while (ec_vector_ch_zip_iter_next(&z, &c1, &c2) == EC_OK) { cnt++; }
	assert(cnt == 3);
	assert(ec_vector_ch_zip_iter_remove(&z) == EC_OK);
	assert(ec_vector_ch_zip_iter_add(&z, 'q', 'w') == EC_OK);
	char r1, r2; assert(ec_vector_ch_zip_iter_replace(&z, 'e', 'r', &r1, &r2) == EC_OK);
	a.free_fn(&a); b.free_fn(&b);
	printf("Passed test: vector_char_zip_iterators\n");
}

int
main(void)
{
	printf("Starting ec_vector_char tests...\n");
	void* mptr = NULL;
	test_vector_char_push_get(mptr);
	test_vector_char_insert_remove_swap(mptr);
	test_vector_char_contains_index_binary(mptr);
	test_vector_char_map_reduce_filter(mptr);
	test_vector_char_copy_subarray_trim(mptr);
	test_vector_char_iterators(mptr);
	test_vector_char_zip_iterators(mptr);
	printf("All ec_vector_char tests passed.\n");
	return 0;
}
