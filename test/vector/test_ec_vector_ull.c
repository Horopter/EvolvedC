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
cmp_ull(const void* a, const void* b)
{
	unsigned long long A = *(const unsigned long long*)a, B = *(const unsigned long long*)b;
	return (A > B) - (A < B);
}
static void
map_inc_ull(unsigned long long* x)
{
	(*x)++;
}
static void
red_sum_ull(unsigned long long* acc, unsigned long long* e, unsigned long long* out)
{
	*out = *acc + *e;
}
static int
pred_even_ull(unsigned long long x)
{
	return (x & 1ULL) == 0;
}
static unsigned long long
copy_ull(unsigned long long* src)
{
	return *src;
}

int
main(void)
{
	printf("Starting ec_vector_ull tests...\n");
	ec_vector_ull_t v;
	assert(ec_vector_ull_init(&v, 8, NULL) == EC_OK);
	assert(v.push(&v, 3ULL) == EC_OK);
	assert(v.push(&v, 1ULL) == EC_OK);
	assert(v.insert(&v, 1, 2ULL) == EC_OK);
	unsigned long long out;
	assert(v.get(&v, 0, &out) == EC_OK);
	assert(v.remove_at(&v, 0) == EC_OK);

	unsigned long long replaced;
	assert(v.replace_at(&v, 0, 5ULL, &replaced) == EC_OK);
	assert(v.push(&v, 7ULL) == EC_OK);
	assert(v.swap(&v, 0, 1) == EC_OK);
	size_t has = v.contains(&v, 7ULL, cmp_ull);
	assert(has == 1);
	size_t idx;
	assert(v.index_of(&v, 5ULL, &idx, cmp_ull) == EC_OK);

	v.sort(&v, cmp_ull);
	v.reverse(&v);
	assert(v.binary_search(&v, 5ULL, cmp_ull, &idx) == EC_OK);

	v.map(&v, map_inc_ull);
	unsigned long long acc = 0;
	v.reduce(&v, red_sum_ull, &acc);

	if (v.size >= 2)
		{
			ec_vector_ull_t sub;
			assert(v.subarray(&v, 0, v.size - 1, &sub) == EC_OK);
			sub.free_fn(&sub);
		}
	ec_vector_ull_t s1;
	assert(v.copy_shallow(&v, &s1) == EC_OK);
	s1.free_fn(&s1);
	ec_vector_ull_t s2;
	assert(v.copy_deep(&v, copy_ull, &s2) == EC_OK);
	s2.free_fn(&s2);

	ec_vector_ull_t filt;
	assert(v.filter(&v, pred_even_ull, &filt) == EC_OK);
	filt.free_fn(&filt);
	assert(v.filter_mut(&v, pred_even_ull) == EC_OK);

	(void)v.size_fn(&v);
	(void)v.capacity_fn(&v);
	assert(v.trim_capacity(&v) == EC_OK);

	ec_vector_ull_iter_t it;
	assert(ec_vector_ull_iter_init(&it, &v) == EC_OK);
	unsigned long long tmp;
	while (ec_vector_ull_iter_next(&it, &tmp) == EC_OK)
		{
		}
	(void)ec_vector_ull_iter_remove(&it);
	(void)ec_vector_ull_iter_add(&it, 9ULL);
	(void)ec_vector_ull_iter_replace(&it, 8ULL, &replaced);
	ec_vector_ull_t v2;
	assert(ec_vector_ull_init(&v2, 2, NULL) == EC_OK);
	v2.push(&v2, 1ULL);
	v2.push(&v2, 2ULL);
	ec_vector_ull_zip_iter_t zip;
	assert(ec_vector_ull_zip_iter_init(&zip, &v, &v2) == EC_OK);
	unsigned long long z1, z2;
	while (ec_vector_ull_zip_iter_next(&zip, &z1, &z2) == EC_OK)
		{
		}
	(void)ec_vector_ull_zip_iter_remove(&zip);
	(void)ec_vector_ull_zip_iter_add(&zip, 1ULL, 2ULL);
	(void)ec_vector_ull_zip_iter_replace(&zip, 2ULL, 1ULL, &z1, &z2);

	v2.free_fn(&v2);
	v.free_fn(&v);
	printf("All ec_vector_ull tests passed.\n");
	return 0;
}
