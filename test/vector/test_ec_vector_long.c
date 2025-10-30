#include "../../src/ec_vector.h"
#include <assert.h>
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
cmp_long(const void* a, const void* b)
{
	long A = *(const long*)a, B = *(const long*)b;
	return (A > B) - (A < B);
}
static void
map_inc_long(long* x)
{
	(*x)++;
}
static void
red_sum_long(long* acc, long* e, long* out)
{
	*out = *acc + *e;
}
static int
pred_even_long(long x)
{
	return (x & 1L) == 0;
}
static long
copy_long(long* src)
{
	return *src;
}

int
main(void)
{
	printf("Starting ec_vector_long tests...\n");
	ec_vector_long_t v;
	assert(ec_vector_long_init(&v, 8, NULL) == EC_OK);
	assert(v.push(&v, 3L) == EC_OK);
	assert(v.push(&v, 1L) == EC_OK);
	assert(v.insert(&v, 1, 2L) == EC_OK);
	long out;
	assert(v.get(&v, 0, &out) == EC_OK);
	assert(v.remove_at(&v, 0) == EC_OK);

	long replaced;
	assert(v.replace_at(&v, 0, 5L, &replaced) == EC_OK);
	assert(v.push(&v, 7L) == EC_OK);
	assert(v.swap(&v, 0, 1) == EC_OK);
	size_t has = v.contains(&v, 7L, cmp_long);
	assert(has == 1);
	size_t idx;
	assert(v.index_of(&v, 5L, &idx, cmp_long) == EC_OK);

	v.sort(&v, cmp_long);
	v.reverse(&v);
	assert(v.binary_search(&v, 5L, cmp_long, &idx) == EC_OK);

	v.map(&v, map_inc_long);
	long acc = 0;
	v.reduce(&v, red_sum_long, &acc);

	if (v.size >= 2)
		{
			ec_vector_long_t sub;
			assert(v.subarray(&v, 0, v.size - 1, &sub) == EC_OK);
			sub.free_fn(&sub);
		}
	ec_vector_long_t s1;
	assert(v.copy_shallow(&v, &s1) == EC_OK);
	s1.free_fn(&s1);
	ec_vector_long_t s2;
	assert(v.copy_deep(&v, copy_long, &s2) == EC_OK);
	s2.free_fn(&s2);

	ec_vector_long_t filt;
	assert(v.filter(&v, pred_even_long, &filt) == EC_OK);
	filt.free_fn(&filt);
	assert(v.filter_mut(&v, pred_even_long) == EC_OK);

	(void)v.size_fn(&v);
	(void)v.capacity_fn(&v);
	assert(v.trim_capacity(&v) == EC_OK);

	ec_vector_long_iter_t it;
	assert(ec_vector_long_iter_init(&it, &v) == EC_OK);
	long tmp;
	while (ec_vector_long_iter_next(&it, &tmp) == EC_OK)
		{
		}
	size_t prev_sz = v.size;
	assert(ec_vector_long_iter_remove(&it) == EC_OK);
	assert(v.size == prev_sz - 1);
	prev_sz = v.size;
	assert(ec_vector_long_iter_add(&it, 9L) == EC_OK);
	assert(v.size == prev_sz + 1);
	assert(ec_vector_long_iter_replace(&it, 8L, &replaced) == EC_OK);
	size_t idxr;
	assert(v.index_of(&v, 8L, &idxr, cmp_long) == EC_OK);
	ec_vector_long_t v2;
	assert(ec_vector_long_init(&v2, 2, NULL) == EC_OK);
	v2.push(&v2, 1L);
	v2.push(&v2, 2L);
	ec_vector_long_zip_iter_t zip;
	assert(ec_vector_long_zip_iter_init(&zip, &v, &v2) == EC_OK);
	long z1, z2;
	while (ec_vector_long_zip_iter_next(&zip, &z1, &z2) == EC_OK)
		{
		}
	size_t sz1 = v.size, sz2 = v2.size;
	assert(ec_vector_long_zip_iter_remove(&zip) == EC_OK);
	assert(v.size == sz1 - 1 && v2.size == sz2 - 1);
	sz1 = v.size;
	sz2 = v2.size;
	assert(ec_vector_long_zip_iter_add(&zip, 1L, 2L) == EC_OK);
	assert(v.size == sz1 + 1 && v2.size == sz2 + 1);
	size_t zi = ec_vector_long_zip_iter_index(&zip);
	assert(v.get(&v, zi, &z1) == EC_OK && z1 == 1L);
	assert(v2.get(&v2, zi, &z2) == EC_OK && z2 == 2L);
	long prev1, prev2;
	assert(ec_vector_long_zip_iter_replace(&zip, 2L, 1L, &prev1, &prev2) == EC_OK);
	assert(v.get(&v, zi, &z1) == EC_OK);
	assert(v2.get(&v2, zi, &z2) == EC_OK);

	v2.free_fn(&v2);
	v.free_fn(&v);
	printf("All ec_vector_long tests passed.\n");
	return 0;
}
