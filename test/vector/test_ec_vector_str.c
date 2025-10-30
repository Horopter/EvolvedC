#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
cmp_cstr(const void* a, const void* b)
{
	const char* A = *(const char* const*)a;
	const char* B = *(const char* const*)b;
	return strcmp(A, B);
}
static void
map_noop_str(const char** s)
{
	(void)s;
}
static void
red_total_len(size_t* acc, const char** e, size_t* out)
{
	*out = *acc + strlen(*e);
}
static int
pred_nonempty(const char* s)
{
	return s && *s;
}
static const char*
copy_str(const char** src)
{
	return *src;
}

int
main(void)
{
	printf("Starting ec_vector_str tests...\n");
	ec_vector_str_t v;
	assert(ec_vector_str_init(&v, 8, NULL) == EC_OK);
	assert(v.push(&v, "banana") == EC_OK);
	assert(v.push(&v, "apple") == EC_OK);
	assert(v.insert(&v, 1, "cherry") == EC_OK);
	const char* out;
	assert(v.get(&v, 0, &out) == EC_OK);
	assert(v.remove_at(&v, 0) == EC_OK);

	const char* replaced_s;
	assert(v.replace_at(&v, 0, "date", &replaced_s) == EC_OK);
	assert(v.push(&v, "elderberry") == EC_OK);
	assert(v.swap(&v, 0, 1) == EC_OK);
	size_t has = v.contains(&v, "elderberry", cmp_cstr);
	assert(has == 1);
	size_t idx;
	assert(v.index_of(&v, "date", &idx, cmp_cstr) == EC_OK);

	v.sort(&v, cmp_cstr);
	v.reverse(&v);
	assert(v.binary_search(&v, "date", cmp_cstr, &idx) == EC_OK);

	v.map(&v, map_noop_str);
	size_t acc = 0;
	v.reduce(&v, red_total_len, &acc);

	if (v.size >= 2)
		{
			ec_vector_str_t sub;
			assert(v.subarray(&v, 0, v.size - 1, &sub) == EC_OK);
			sub.free_fn(&sub);
		}
	ec_vector_str_t s1;
	assert(v.copy_shallow(&v, &s1) == EC_OK);
	s1.free_fn(&s1);
	ec_vector_str_t s2;
	assert(v.copy_deep(&v, copy_str, &s2) == EC_OK);
	s2.free_fn(&s2);

	ec_vector_str_t filt;
	assert(v.filter(&v, pred_nonempty, &filt) == EC_OK);
	filt.free_fn(&filt);
	assert(v.filter_mut(&v, pred_nonempty) == EC_OK);

	(void)v.size_fn(&v);
	(void)v.capacity_fn(&v);
	assert(v.trim_capacity(&v) == EC_OK);

	ec_vector_str_iter_t it;
	assert(ec_vector_str_iter_init(&it, &v) == EC_OK);
	const char* tmp;
	while (ec_vector_str_iter_next(&it, &tmp) == EC_OK)
		{
		}
	(void)ec_vector_str_iter_remove(&it);
	(void)ec_vector_str_iter_add(&it, "fig");
	(void)ec_vector_str_iter_replace(&it, "grape", &out);
	ec_vector_str_t v2;
	assert(ec_vector_str_init(&v2, 2, NULL) == EC_OK);
	v2.push(&v2, "a");
	v2.push(&v2, "b");
	ec_vector_str_zip_iter_t zip;
	assert(ec_vector_str_zip_iter_init(&zip, &v, &v2) == EC_OK);
	const char *z1, *z2;
	while (ec_vector_str_zip_iter_next(&zip, &z1, &z2) == EC_OK)
		{
		}
	(void)ec_vector_str_zip_iter_remove(&zip);
	(void)ec_vector_str_zip_iter_add(&zip, "h", "i");
	(void)ec_vector_str_zip_iter_replace(&zip, "j", "k", &z1, &z2);

	v2.free_fn(&v2);
	v.free_fn(&v);
	printf("All ec_vector_str tests passed.\n");
	return 0;
}
