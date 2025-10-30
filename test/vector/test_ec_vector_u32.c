#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdint.h>
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
		printf("[timeerr] ");
}
static int
cmp_u32(const void* a, const void* b)
{
	uint32_t A = *(const uint32_t*)a, B = *(const uint32_t*)b;
	return (A > B) - (A < B);
}
static void
map_inc_u32(uint32_t* x)
{
	(*x)++;
}
static void
red_sum_u32(uint32_t* acc, uint32_t* e, uint32_t* out)
{
	*out = *acc + *e;
}
static int
copy_u32(uint32_t* src)
{
	return *src;
}
static int
is_even_u32(uint32_t x)
{
	return (x & 1u) == 0;
}

int
main(void)
{
	printf("Starting ec_vector_u32 tests...\n");
	void* mptr = NULL;

	ec_vector_u32_t v;
	assert(ec_vector_u32_init(&v, 8, mptr) == EC_OK);
	/* push, insert, get, remove_at */
	assert(v.push(&v, 3u) == EC_OK);
	assert(v.push(&v, 1u) == EC_OK);
	assert(v.insert(&v, 1, 2u) == EC_OK);
	uint32_t out;
	assert(v.get(&v, 0, &out) == EC_OK && out == 3u);
	assert(v.remove_at(&v, 0) == EC_OK);
	assert(v.get(&v, 0, &out) == EC_OK && out == 2u);

	/* replace_at, swap, contains, index_of */
	uint32_t replaced;
	assert(v.replace_at(&v, 0, 5u, &replaced) == EC_OK);
	assert(v.push(&v, 7u) == EC_OK);
	assert(v.swap(&v, 0, 1) == EC_OK);
	size_t has = v.contains(&v, 7u, cmp_u32);
	assert(has == 1);
	size_t idx;
	assert(v.index_of(&v, 5u, &idx, cmp_u32) == EC_OK);

	/* sort, reverse, binary_search */
	v.sort(&v, cmp_u32);
	v.reverse(&v);
	assert(v.binary_search(&v, 5u, cmp_u32, &idx) == EC_OK);

	/* map, reduce */
	v.map(&v, map_inc_u32);
	uint32_t acc = 0;
	v.reduce(&v, red_sum_u32, &acc);

	/* subarray, shallow/deep copy */
	if (v.size >= 2)
		{
			ec_vector_u32_t sub;
			assert(v.subarray(&v, 0, v.size - 1, &sub) == EC_OK);
			sub.free_fn(&sub);
		}
	ec_vector_u32_t s1;
	assert(v.copy_shallow(&v, &s1) == EC_OK);
	s1.free_fn(&s1);
	ec_vector_u32_t s2;
	assert(v.copy_deep(&v, copy_u32, &s2) == EC_OK);
	s2.free_fn(&s2);

	/* filter, filter_mut */
	ec_vector_u32_t filt;
	assert(v.filter(&v, is_even_u32, &filt) == EC_OK);
	filt.free_fn(&filt);
	assert(v.filter_mut(&v, is_even_u32) == EC_OK);

	/* size/capacity/trim */
	(void)v.size_fn(&v);
	(void)v.capacity_fn(&v);
	assert(v.trim_capacity(&v) == EC_OK);

	/* iter and zip iter */
	ec_vector_u32_iter_t it;
	assert(ec_vector_u32_iter_init(&it, &v) == EC_OK);
	uint32_t tmp;
	while (ec_vector_u32_iter_next(&it, &tmp) == EC_OK)
		{
		}
	(void)ec_vector_u32_iter_remove(&it);
	(void)ec_vector_u32_iter_add(&it, 9u);
	(void)ec_vector_u32_iter_replace(&it, 8u, &replaced);
	ec_vector_u32_t v2;
	assert(ec_vector_u32_init(&v2, 2, NULL) == EC_OK);
	v2.push(&v2, 1u);
	v2.push(&v2, 2u);
	ec_vector_u32_zip_iter_t zip;
	assert(ec_vector_u32_zip_iter_init(&zip, &v, &v2) == EC_OK);
	uint32_t z1, z2;
	while (ec_vector_u32_zip_iter_next(&zip, &z1, &z2) == EC_OK)
		{
		}
	(void)ec_vector_u32_zip_iter_remove(&zip);
	(void)ec_vector_u32_zip_iter_add(&zip, 1u, 2u);
	(void)ec_vector_u32_zip_iter_replace(&zip, 2u, 1u, &z1, &z2);

	v2.free_fn(&v2);
	v.free_fn(&v);
	printf("All ec_vector_u32 tests passed.\n");
	return 0;
}
