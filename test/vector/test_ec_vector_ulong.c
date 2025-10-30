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
cmp_ulong(const void* a, const void* b)
{
	unsigned long A = *(const unsigned long*)a, B = *(const unsigned long*)b;
	return (A > B) - (A < B);
}
static void
map_inc_ul(unsigned long* x)
{
	(*x)++;
}
static void
red_sum_ul(unsigned long* acc, unsigned long* e, unsigned long* out)
{
	*out = *acc + *e;
}
static int
pred_even_ul(unsigned long x)
{
	return (x & 1UL) == 0;
}
static unsigned long
copy_ul(unsigned long* src)
{
	return *src;
}

int
main(void)
{
	printf("Starting ec_vector_ulong tests...\n");
	ec_vector_ul_t v;
	assert(ec_vector_ul_init(&v, 8, NULL) == EC_OK);
	assert(v.push(&v, 10UL) == EC_OK);
	assert(v.push(&v, 5UL) == EC_OK);
	assert(v.insert(&v, 1, 7UL) == EC_OK);
	unsigned long out;
	assert(v.get(&v, 0, &out) == EC_OK);
	assert(v.remove_at(&v, 0) == EC_OK);

	unsigned long replaced;
	assert(v.replace_at(&v, 0, 11UL, &replaced) == EC_OK);
	assert(v.push(&v, 13UL) == EC_OK);
	assert(v.swap(&v, 0, 1) == EC_OK);
	size_t has = v.contains(&v, 13UL, cmp_ulong);
	assert(has == 1);
	size_t idx;
	assert(v.index_of(&v, 11UL, &idx, cmp_ulong) == EC_OK);

	v.sort(&v, cmp_ulong);
	v.reverse(&v);
	assert(v.binary_search(&v, 11UL, cmp_ulong, &idx) == EC_OK);

	v.map(&v, map_inc_ul);
	unsigned long acc = 0;
	v.reduce(&v, red_sum_ul, &acc);

	if (v.size >= 2)
		{
			ec_vector_ul_t sub;
			assert(v.subarray(&v, 0, v.size - 1, &sub) == EC_OK);
			sub.free_fn(&sub);
		}
	ec_vector_ul_t s1;
	assert(v.copy_shallow(&v, &s1) == EC_OK);
	s1.free_fn(&s1);
	ec_vector_ul_t s2;
	assert(v.copy_deep(&v, copy_ul, &s2) == EC_OK);
	s2.free_fn(&s2);

	ec_vector_ul_t filt;
	assert(v.filter(&v, pred_even_ul, &filt) == EC_OK);
	filt.free_fn(&filt);
	assert(v.filter_mut(&v, pred_even_ul) == EC_OK);

	(void)v.size_fn(&v);
	(void)v.capacity_fn(&v);
	assert(v.trim_capacity(&v) == EC_OK);

	ec_vector_ul_iter_t it;
	assert(ec_vector_ul_iter_init(&it, &v) == EC_OK);
	unsigned long tmp;
	while (ec_vector_ul_iter_next(&it, &tmp) == EC_OK)
		{
		}
	(void)ec_vector_ul_iter_remove(&it);
	(void)ec_vector_ul_iter_add(&it, 19UL);
	(void)ec_vector_ul_iter_replace(&it, 17UL, &replaced);
	ec_vector_ul_t v2;
	assert(ec_vector_ul_init(&v2, 2, NULL) == EC_OK);
	v2.push(&v2, 1UL);
	v2.push(&v2, 2UL);
	ec_vector_ul_zip_iter_t zip;
	assert(ec_vector_ul_zip_iter_init(&zip, &v, &v2) == EC_OK);
	unsigned long z1, z2;
	while (ec_vector_ul_zip_iter_next(&zip, &z1, &z2) == EC_OK)
		{
		}
	(void)ec_vector_ul_zip_iter_remove(&zip);
	(void)ec_vector_ul_zip_iter_add(&zip, 1UL, 2UL);
	(void)ec_vector_ul_zip_iter_replace(&zip, 2UL, 1UL, &z1, &z2);

	v2.free_fn(&v2);
	v.free_fn(&v);
	printf("All ec_vector_ulong tests passed.\n");
	return 0;
}
