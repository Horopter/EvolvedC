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
cmp_ll(const void* a, const void* b)
{
	long long A = *(const long long*)a, B = *(const long long*)b;
	return (A > B) - (A < B);
}
static void
map_inc_ll(long long* x)
{
	(*x)++;
}
static void
red_sum_ll(long long* acc, long long* e, long long* out)
{
	*out = *acc + *e;
}
static int
pred_even_ll(long long x)
{
	return (x & 1LL) == 0;
}
static long long
copy_ll(long long* src)
{
	return *src;
}

int
main(void)
{
	printf("Starting ec_vector_ll tests...\n");
	ec_vector_ll_t v;
	assert(ec_vector_ll_init(&v, 8, NULL) == EC_OK);
	/* push/insert/get/remove */
	assert(v.push(&v, 3) == EC_OK);
	assert(v.push(&v, 1) == EC_OK);
	assert(v.insert(&v, 1, 2) == EC_OK);
	long long out;
	assert(v.get(&v, 0, &out) == EC_OK);
	assert(v.remove_at(&v, 0) == EC_OK);

	/* replace/swap/contains/index_of */
	long long replaced;
	assert(v.replace_at(&v, 0, 5, &replaced) == EC_OK);
	assert(v.push(&v, 7) == EC_OK);
	assert(v.swap(&v, 0, 1) == EC_OK);
	size_t has = v.contains(&v, 7, cmp_ll);
	assert(has == 1);
	size_t idx;
	assert(v.index_of(&v, 5, &idx, cmp_ll) == EC_OK);

	/* sort/reverse/binary_search */
	v.sort(&v, cmp_ll);
	v.reverse(&v);
	assert(v.binary_search(&v, 5, cmp_ll, &idx) == EC_OK);

	/* map/reduce */
	v.map(&v, map_inc_ll);
	long long acc = 0;
	v.reduce(&v, red_sum_ll, &acc);

	/* subarray/copies */
	if (v.size >= 2)
		{
			ec_vector_ll_t sub;
			assert(v.subarray(&v, 0, v.size - 1, &sub) == EC_OK);
			sub.free_fn(&sub);
		}
	ec_vector_ll_t s1;
	assert(v.copy_shallow(&v, &s1) == EC_OK);
	s1.free_fn(&s1);
	ec_vector_ll_t s2;
	assert(v.copy_deep(&v, copy_ll, &s2) == EC_OK);
	s2.free_fn(&s2);

	/* filter/filter_mut */
	ec_vector_ll_t filt;
	assert(v.filter(&v, pred_even_ll, &filt) == EC_OK);
	filt.free_fn(&filt);
	assert(v.filter_mut(&v, pred_even_ll) == EC_OK);

	/* size/capacity/trim */
	(void)v.size_fn(&v);
	(void)v.capacity_fn(&v);
	assert(v.trim_capacity(&v) == EC_OK);

	/* iter/zip */
	ec_vector_ll_iter_t it;
	assert(ec_vector_ll_iter_init(&it, &v) == EC_OK);
	long long tmp;
	while (ec_vector_ll_iter_next(&it, &tmp) == EC_OK)
		{
		}
	(void)ec_vector_ll_iter_remove(&it);
	(void)ec_vector_ll_iter_add(&it, 9);
	(void)ec_vector_ll_iter_replace(&it, 8, &replaced);
	ec_vector_ll_t v2;
	assert(ec_vector_ll_init(&v2, 2, NULL) == EC_OK);
	v2.push(&v2, 1);
	v2.push(&v2, 2);
	ec_vector_ll_zip_iter_t zip;
	assert(ec_vector_ll_zip_iter_init(&zip, &v, &v2) == EC_OK);
	long long z1, z2;
	while (ec_vector_ll_zip_iter_next(&zip, &z1, &z2) == EC_OK)
		{
		}
	(void)ec_vector_ll_zip_iter_remove(&zip);
	(void)ec_vector_ll_zip_iter_add(&zip, 1, 2);
	(void)ec_vector_ll_zip_iter_replace(&zip, 2, 1, &z1, &z2);

	v2.free_fn(&v2);
	v.free_fn(&v);
	printf("All ec_vector_ll tests passed.\n");
	return 0;
}
