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
cmp_ptr(const void* a, const void* b)
{
	const void* A = *(void* const*)a;
	const void* B = *(void* const*)b;
	return (A > B) - (A < B);
}
static void
map_noop(void** p)
{
	(void)p;
}
static void
red_count(size_t* acc, void** elem, size_t* out)
{
	(void)elem;
	*out = *acc + 1;
}
static int
pred_nonnull(void* p)
{
	return p != NULL;
}
static void*
copy_ptr(void** src)
{
	return *src;
}

int
main(void)
{
	printf("Starting ec_vector_ptr tests...\n");
	ec_vector_ptr_t v;
	assert(ec_vector_ptr_init(&v, 4, NULL) == EC_OK);
	int a = 1, b = 2, c = 3;
	void *pa = &a, *pb = &b, *pc = &c;
	assert(v.push(&v, pa) == EC_OK);
	assert(v.insert(&v, 1, pb) == EC_OK);
	assert(v.push(&v, pc) == EC_OK);

	void* out;
	assert(v.get(&v, 0, &out) == EC_OK && out == pa);
	assert(v.remove_at(&v, 1) == EC_OK);
	assert(v.replace_at(&v, 1, pb, &out) == EC_OK);
	assert(v.swap(&v, 0, 1) == EC_OK);

	size_t has = v.contains(&v, pb, cmp_ptr);
	assert(has == 1);
	size_t idx;
	assert(v.index_of(&v, pb, &idx, cmp_ptr) == EC_OK);
	v.sort(&v, cmp_ptr);
	v.reverse(&v);
	assert(v.binary_search(&v, pb, cmp_ptr, &idx) == EC_OK);

	v.map(&v, map_noop);
	size_t sum = 0;
	v.reduce(&v, red_count, &sum);

	if (v.size >= 2)
		{
			ec_vector_ptr_t sub;
			assert(v.subarray(&v, 0, v.size - 1, &sub) == EC_OK);
			sub.free_fn(&sub);
		}
	ec_vector_ptr_t s1;
	assert(v.copy_shallow(&v, &s1) == EC_OK);
	s1.free_fn(&s1);
	ec_vector_ptr_t s2;
	assert(v.copy_deep(&v, copy_ptr, &s2) == EC_OK);
	s2.free_fn(&s2);

	ec_vector_ptr_t filt;
	assert(v.filter(&v, pred_nonnull, &filt) == EC_OK);
	filt.free_fn(&filt);
	assert(v.filter_mut(&v, pred_nonnull) == EC_OK);

	(void)v.size_fn(&v);
	(void)v.capacity_fn(&v);
	assert(v.trim_capacity(&v) == EC_OK);

	ec_vector_ptr_iter_t it;
	assert(ec_vector_ptr_iter_init(&it, &v) == EC_OK);
	void* tmp;
	while (ec_vector_ptr_iter_next(&it, &tmp) == EC_OK)
		{
		}
	(void)ec_vector_ptr_iter_remove(&it);
	(void)ec_vector_ptr_iter_add(&it, pa);
	(void)ec_vector_ptr_iter_replace(&it, pb, &out);

	ec_vector_ptr_t v2;
	assert(ec_vector_ptr_init(&v2, 2, NULL) == EC_OK);
	v2.push(&v2, pa);
	v2.push(&v2, pb);
	ec_vector_ptr_zip_iter_t zip;
	assert(ec_vector_ptr_zip_iter_init(&zip, &v, &v2) == EC_OK);
	void *z1, *z2;
	while (ec_vector_ptr_zip_iter_next(&zip, &z1, &z2) == EC_OK)
		{
		}
	(void)ec_vector_ptr_zip_iter_remove(&zip);
	(void)ec_vector_ptr_zip_iter_add(&zip, pa, pb);
	(void)ec_vector_ptr_zip_iter_replace(&zip, pb, pa, &z1, &z2);

	v2.free_fn(&v2);
	v.free_fn(&v);
	printf("All ec_vector_ptr tests passed.\n");
	return 0;
}
