#include "../../src/ec_hashmap.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

static void
for_each_key(const char* k)
{
	(void)k;
}
static void
for_each_val(const char* v)
{
	(void)v;
}

static void
test_init_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	assert(hm.capacity == 16);
	assert(hm.size == 0);
	hm.remove_all(&hm);
}

static void
test_init_neg(void)
{
	assert(ec_hashmap_str_init(NULL, NULL) == EC_ERR_INVALID_ARG);
}

static void
test_add_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k1 = "aa", *v1 = "xx", *k2 = "bb", *v2 = "yy";
	assert(hm.add(&hm, &k1, &v1) == EC_OK);
	assert(hm.add(&hm, &k2, &v2) == EC_OK);
	assert(hm.size_fn(&hm) == 2);
	hm.remove_all(&hm);
}

static void
test_add_neg(void)
{
	ec_hashmap_str_t hm;
	const char *k = "aa", *v = "xx";
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	assert(hm.add(NULL, &k, &v) == EC_ERR_INVALID_ARG);
	assert(hm.add(&hm, NULL, &v) == EC_ERR_INVALID_ARG);
	assert(hm.add(&hm, &k, NULL) == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
}

static void
test_get_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k1 = "aa", *v1 = "xx", *k2 = "bb", *v2 = "yy";
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	const char* out;
	assert(hm.get(&hm, &k1, (char**)&out) == EC_OK && strcmp(out, "xx") == 0);
	free((void*)out);
	assert(hm.get(&hm, &k2, (char**)&out) == EC_OK && strcmp(out, "yy") == 0);
	free((void*)out);
	hm.remove_all(&hm);
}

static void
test_get_neg(void)
{
	ec_hashmap_str_t hm;
	const char *k = "aa", *v = "xx", *missing = "nope";
	const char* out;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	hm.add(&hm, &k, &v);
	assert(hm.get(NULL, &k, (char**)&out) == EC_ERR_INVALID_ARG);
	assert(hm.get(&hm, NULL, (char**)&out) == EC_ERR_INVALID_ARG);
	assert(hm.get(&hm, &k, NULL) == EC_ERR_INVALID_ARG);
	assert(hm.get(&hm, &missing, (char**)&out) == EC_ERR_NOT_FOUND);
	hm.remove_all(&hm);
}

static void
test_remove_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k1 = "aa", *v1 = "xx", *k2 = "bb", *v2 = "yy";
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	assert(hm.remove(&hm, &k1) == EC_OK);
	assert(hm.size_fn(&hm) == 1);
	assert(hm.remove(&hm, &k2) == EC_OK);
	assert(hm.size_fn(&hm) == 0);
	hm.remove_all(&hm);
}

static void
test_remove_neg(void)
{
	ec_hashmap_str_t hm;
	const char *k = "aa", *v = "xx", *missing = "nope";
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	hm.add(&hm, &k, &v);
	assert(hm.remove(NULL, &k) == EC_ERR_INVALID_ARG);
	assert(hm.remove(&hm, NULL) == EC_ERR_INVALID_ARG);
	assert(hm.remove(&hm, &missing) == EC_ERR_NOT_FOUND);
	hm.remove_all(&hm);
}

static void
test_remove_all_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k1 = "aa", *v1 = "xx", *k2 = "bb", *v2 = "yy";
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	assert(hm.remove_all(&hm) == EC_OK);
	assert(hm.size_fn(&hm) == 0);
	assert(hm.remove_all(&hm) == EC_OK);
	hm.remove_all(&hm);
}

static void
test_remove_all_neg(void)
{
	assert(ec_hashmap_str_remove_all_f(NULL) == EC_ERR_INVALID_ARG);
}

static void
test_contains_key_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k = "aa", *v = "xx";
	hm.add(&hm, &k, &v);
	assert(hm.contains_key(&hm, &k) == EC_OK);
	hm.remove_all(&hm);
}

static void
test_contains_key_neg(void)
{
	ec_hashmap_str_t hm;
	const char *k = "aa", *v = "xx", *missing = "nope";
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	hm.add(&hm, &k, &v);
	assert(hm.contains_key(NULL, &k) == EC_ERR_INVALID_ARG);
	assert(hm.contains_key(&hm, NULL) == EC_ERR_INVALID_ARG);
	assert(hm.contains_key(&hm, &missing) == EC_ERR_NOT_FOUND);
	hm.remove_all(&hm);
}

static void
test_size_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	assert(hm.size_fn(&hm) == 0);
	const char *k = "aa", *v = "xx";
	hm.add(&hm, &k, &v);
	assert(hm.size_fn(&hm) == 1);
	hm.remove_all(&hm);
}

static void
test_size_neg(void)
{
	assert(ec_hashmap_str_size_f(NULL) == 0);
}

static void
test_capacity_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	assert(hm.capacity_fn(&hm) == 16);
	hm.remove_all(&hm);
}

static void
test_capacity_neg(void)
{
	assert(ec_hashmap_str_capacity_f(NULL) == 0);
}

static void
test_get_keys_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k1 = "aa", *v1 = "xx", *k2 = "bb", *v2 = "yy", *k3 = "cc", *v3 = "zz";
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	hm.add(&hm, &k3, &v3);
	const char** ks = NULL;
	size_t kc = 0;
	assert(hm.get_keys(&hm, (char**)&ks, &kc) == EC_OK);
	assert(kc == 3);
	if (ks)
		free((void*)ks);
	hm.remove_all(&hm);
}

static void
test_get_keys_neg(void)
{
	ec_hashmap_str_t hm;
	const char** ks = NULL;
	size_t kc = 0;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	assert(hm.get_keys(NULL, (char**)&ks, &kc) == EC_ERR_INVALID_ARG);
	assert(hm.get_keys(&hm, NULL, &kc) == EC_ERR_INVALID_ARG);
	assert(hm.get_keys(&hm, (char**)&ks, NULL) == EC_ERR_INVALID_ARG);
	assert(hm.get_keys(&hm, (char**)&ks, &kc) == EC_OK && kc == 0 && ks == NULL);
	hm.remove_all(&hm);
}

static void
test_get_values_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k1 = "aa", *v1 = "xx", *k2 = "bb", *v2 = "yy", *k3 = "cc", *v3 = "zz";
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	hm.add(&hm, &k3, &v3);
	const char** vs = NULL;
	size_t vc = 0;
	assert(hm.get_values(&hm, (char**)&vs, &vc) == EC_OK);
	assert(vc == 3);
	if (vs)
		free((void*)vs);
	hm.remove_all(&hm);
}

static void
test_get_values_neg(void)
{
	ec_hashmap_str_t hm;
	const char** vs = NULL;
	size_t vc = 0;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	assert(hm.get_values(NULL, (char**)&vs, &vc) == EC_ERR_INVALID_ARG);
	assert(hm.get_values(&hm, NULL, &vc) == EC_ERR_INVALID_ARG);
	assert(hm.get_values(&hm, (char**)&vs, NULL) == EC_ERR_INVALID_ARG);
	assert(hm.get_values(&hm, (char**)&vs, &vc) == EC_OK && vc == 0 && vs == NULL);
	hm.remove_all(&hm);
}

static void
test_foreach_key_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k = "aa", *v = "xx";
	hm.add(&hm, &k, &v);
	assert(hm.foreach_key(&hm, for_each_key) == EC_OK);
	hm.remove_all(&hm);
}

static void
test_foreach_key_neg(void)
{
	ec_hashmap_str_t hm;
	const char *k = "aa", *v = "xx";
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	hm.add(&hm, &k, &v);
	assert(hm.foreach_key(NULL, for_each_key) == EC_ERR_INVALID_ARG);
	assert(hm.foreach_key(&hm, NULL) == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
}

static void
test_foreach_value_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k = "aa", *v = "xx";
	hm.add(&hm, &k, &v);
	assert(hm.foreach_value(&hm, for_each_val) == EC_OK);
	hm.remove_all(&hm);
}

static void
test_foreach_value_neg(void)
{
	ec_hashmap_str_t hm;
	const char *k = "aa", *v = "xx";
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	hm.add(&hm, &k, &v);
	assert(hm.foreach_value(NULL, for_each_val) == EC_ERR_INVALID_ARG);
	assert(hm.foreach_value(&hm, NULL) == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
}

static void
test_iter_init_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	ec_hashmap_str_iter_t it;
	assert(hm.iter_init(&hm, &it) == EC_OK);
	hm.remove_all(&hm);
}

static void
test_iter_init_neg(void)
{
	ec_hashmap_str_t hm;
	ec_hashmap_str_iter_t it;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	assert(hm.iter_init(NULL, &it) == EC_ERR_INVALID_ARG);
	assert(hm.iter_init(&hm, NULL) == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
}

static void
test_iter_next_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k1 = "aa", *v1 = "xx", *k2 = "bb", *v2 = "yy";
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	ec_hashmap_str_iter_t it;
	hm.iter_init(&hm, &it);
	const char *ik, *iv;
	size_t count = 0;
	while (hm.iter_next(&it, &ik, &iv) == EC_OK)
		{
			count++;
		}
	assert(count == 2);
	hm.remove_all(&hm);
}

static void
test_iter_next_neg(void)
{
	ec_hashmap_str_t hm;
	const char *k = "aa", *v = "xx";
	const char *ik, *iv;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	hm.add(&hm, &k, &v);
	ec_hashmap_str_iter_t it;
	hm.iter_init(&hm, &it);
	assert(hm.iter_next(NULL, &ik, &iv) == EC_ERR_INVALID_ARG);
	hm.iter_init(&hm, &it);
	assert(hm.iter_next(&it, NULL, &iv) == EC_ERR_INVALID_ARG);
	hm.iter_init(&hm, &it);
	assert(hm.iter_next(&it, &ik, NULL) == EC_ERR_INVALID_ARG);
	ec_hashmap_str_t hm2;
	ec_hashmap_str_iter_t it2;
	assert(ec_hashmap_str_init(&hm2, NULL) == EC_OK);
	hm2.iter_init(&hm2, &it2);
	assert(hm2.iter_next(&it2, &ik, &iv) == EC_ERR_OUT_OF_BOUNDS);
	hm.remove_all(&hm);
	hm2.remove_all(&hm2);
}

static void
test_iter_remove_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k1 = "aa", *v1 = "xx", *k2 = "bb", *v2 = "yy";
	hm.add(&hm, &k1, &v1);
	hm.add(&hm, &k2, &v2);
	ec_hashmap_str_iter_t it;
	hm.iter_init(&hm, &it);
	const char *ik, *iv;
	assert(hm.iter_next(&it, &ik, &iv) == EC_OK);
	assert(hm.iter_remove(&it) == EC_OK);
	assert(hm.size_fn(&hm) == 1);
	hm.remove_all(&hm);
}

static void
test_iter_remove_neg(void)
{
	ec_hashmap_str_t hm;
	ec_hashmap_str_iter_t it;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	assert(hm.iter_remove(NULL) == EC_ERR_INVALID_ARG);
	hm.iter_init(&hm, &it);
	assert(hm.iter_remove(&it) == EC_ERR_INVALID_ARG);
	hm.remove_all(&hm);
}

static void
test_duplicate_keys_pos(void)
{
	ec_hashmap_str_t hm;
	assert(ec_hashmap_str_init(&hm, NULL) == EC_OK);
	const char *k = "aa", *v1 = "xx", *v2 = "zz";
	hm.add(&hm, &k, &v1);
	const char* out;
	assert(hm.get(&hm, &k, (char**)&out) == EC_OK && strcmp(out, "xx") == 0);
	free((void*)out);
	hm.add(&hm, &k, &v2);
	assert(hm.get(&hm, &k, (char**)&out) == EC_OK && strcmp(out, "zz") == 0);
	free((void*)out);
	hm.remove_all(&hm);
}

int
main(void)
{
	printf("Starting ec_hashmap_str tests...\n");
	test_init_pos();
	test_init_neg();
	test_add_pos();
	test_add_neg();
	test_get_pos();
	test_get_neg();
	test_remove_pos();
	test_remove_neg();
	test_remove_all_pos();
	test_remove_all_neg();
	test_contains_key_pos();
	test_contains_key_neg();
	test_size_pos();
	test_size_neg();
	test_capacity_pos();
	test_capacity_neg();
	test_get_keys_pos();
	test_get_keys_neg();
	test_get_values_pos();
	test_get_values_neg();
	test_foreach_key_pos();
	test_foreach_key_neg();
	test_foreach_value_pos();
	test_foreach_value_neg();
	test_iter_init_pos();
	test_iter_init_neg();
	test_iter_next_pos();
	test_iter_next_neg();
	test_iter_remove_pos();
	test_iter_remove_neg();
	test_duplicate_keys_pos();
	printf("All ec_hashmap_str tests passed.\n");
	return 0;
}
