#include "../../src/ec_vector.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
cmp_double(const void* a, const void* b)
{
	double da = *(const double*)a;
	double db = *(const double*)b;
	if (da < db)
		return -1;
	if (da > db)
		return 1;
	return 0;
}

static void
scale_double(double* v)
{
	*v *= 1.5;
}
static void
sum_double(double* acc, double* elem, double* out)
{
	*out = (*acc) + (*elem);
}
static bool
is_positive_double(const double v)
{
	return v > 0.0;
}
static double
copy_double_val(double* src)
{
	return *src;
}

static void
test_vector_double_basics(void)
{
	print_timestamp();
	printf("Running test: vector_double_basics\n");

	ec_vector_dbl_t vec;
	ec_status_t st = ec_vector_dbl_init(&vec, 4, NULL);
	assert(st == EC_OK);

	double vals[] = { 3.14, -2.5, 0.0, 10.0, 7.25 };
	for (size_t i = 0; i < sizeof(vals) / sizeof(vals[0]); i++)
		{
			st = vec.push(&vec, vals[i]);
			assert(st == EC_OK);
		}

	double v;
	st = vec.get(&vec, 0, &v);
	assert(st == EC_OK && v == 3.14);
	st = vec.get(&vec, 4, &v);
	assert(st == EC_OK && v == 7.25);

	st = vec.replace_at(&vec, 1, -3.5, &v);
	assert(st == EC_OK && v == -2.5);

	st = vec.swap(&vec, 0, 1);
	assert(st == EC_OK);

	st = vec.remove_at(&vec, 2);
	assert(st == EC_OK);

	vec.free_fn(&vec);
	printf("Passed test: vector_double_basics\n");
}

static void
test_vector_double_algorithms(void)
{
	print_timestamp();
	printf("Running test: vector_double_algorithms\n");

	ec_vector_dbl_t vec;
	ec_status_t st = ec_vector_dbl_init(&vec, 8, NULL);
	assert(st == EC_OK);

	double vals[] = { 5.0, 1.0, 3.0, 2.0, 4.0 };
	for (int i = 0; i < 5; i++)
		{
			assert(vec.push(&vec, vals[i]) == EC_OK);
		}

	vec.sort(&vec, cmp_double);
	double v;
	st = vec.get(&vec, 0, &v);
	assert(st == EC_OK && v == 1.0);
	st = vec.get(&vec, 4, &v);
	assert(st == EC_OK && v == 5.0);

	size_t idx = 999;
	st = vec.binary_search(&vec, 3.0, cmp_double, &idx);
	assert(st == EC_OK && idx < vec.size_fn(&vec));

	vec.reverse(&vec);
	st = vec.get(&vec, 0, &v);
	assert(st == EC_OK && v == 5.0);

	vec.map(&vec, scale_double);
	st = vec.get(&vec, 0, &v);
	assert(st == EC_OK && v == 7.5);

	double total = 0.0;
	vec.reduce(&vec, sum_double, &total);
	assert(total > 0.0);

	/* contains/index_of */
	size_t has = vec.contains(&vec, 7.5, cmp_double);
	assert(has == 1);
	size_t id;
	st = vec.index_of(&vec, 7.5, &id, cmp_double);
	assert(st == EC_OK);

	/* subarray */
	if (vec.size >= 2)
		{
			ec_vector_dbl_t sub;
			st = vec.subarray(&vec, 0, vec.size - 1, &sub);
			assert(st == EC_OK && sub.size == vec.size - 1);
			sub.free_fn(&sub);
		}

	/* copy_shallow / copy_deep */
	ec_vector_dbl_t c1;
	st = vec.copy_shallow(&vec, &c1);
	assert(st == EC_OK && c1.size == vec.size);
	c1.free_fn(&c1);
	ec_vector_dbl_t c2;
	st = vec.copy_deep(&vec, copy_double_val, &c2);
	c2.free_fn(&c2);

	/* filter and filter_mut */
	ec_vector_dbl_t filtered;
	st = vec.filter(&vec, is_positive_double, &filtered);
	assert(st == EC_OK);
	filtered.free_fn(&filtered);

	st = vec.filter_mut(&vec, is_positive_double);
	assert(st == EC_OK);

	/* size/capacity/trim */
	(void)vec.size_fn(&vec);
	(void)vec.capacity_fn(&vec);
	st = vec.trim_capacity(&vec);
	assert(st == EC_OK && vec.capacity == vec.size);

	/* iter and zip-iter */
	ec_vector_dbl_iter_t it;
	st = ec_vector_dbl_iter_init(&it, &vec);
	assert(st == EC_OK);
	double tmp;
	while ((st = ec_vector_dbl_iter_next(&it, &tmp)) == EC_OK)
		{
		}
	size_t prev = vec.size;
	st = ec_vector_dbl_iter_remove(&it);
	assert(st == EC_OK && vec.size == prev - 1);
	prev = vec.size;
	st = ec_vector_dbl_iter_add(&it, 1.0);
	assert(st == EC_OK && vec.size == prev + 1);
	double repv;
	st = ec_vector_dbl_iter_replace(&it, 2.0, &repv);
	assert(st == EC_OK);

	ec_vector_dbl_t v2;
	st = ec_vector_dbl_init(&v2, 4, NULL);
	assert(st == EC_OK);
	(void)v2.push(&v2, 10.0);
	(void)v2.push(&v2, 20.0);
	ec_vector_dbl_zip_iter_t zip;
	st = ec_vector_dbl_zip_iter_init(&zip, &vec, &v2);
	assert(st == EC_OK);
	double z1, z2;
	while ((st = ec_vector_dbl_zip_iter_next(&zip, &z1, &z2)) == EC_OK)
		{
		}
	size_t sv1 = vec.size, sv2 = v2.size;
	st = ec_vector_dbl_zip_iter_remove(&zip);
	assert(st == EC_OK && vec.size == sv1 - 1 && v2.size == sv2 - 1);
	sv1 = vec.size;
	sv2 = v2.size;
	st = ec_vector_dbl_zip_iter_add(&zip, 3.0, 30.0);
	assert(st == EC_OK && vec.size == sv1 + 1 && v2.size == sv2 + 1);
	double old1, old2;
	st = ec_vector_dbl_zip_iter_replace(&zip, 4.0, 40.0, &old1, &old2);
	assert(st == EC_OK);
	v2.free_fn(&v2);

	vec.free_fn(&vec);
	printf("Passed test: vector_double_algorithms\n");
}

int
main(void)
{
	printf("Starting ec_vector_double tests...\n");
	test_vector_double_basics();
	test_vector_double_algorithms();
	printf("All ec_vector_double tests passed.\n");
	return 0;
}
