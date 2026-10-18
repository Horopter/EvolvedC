#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void
test_init_pos(void)
{
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 2, NULL));
	ec_buf_str_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_str_init(NULL, 2, NULL));
}

static void
test_put_peek_get_pos(void)
{
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 2, NULL));
	const char *s1 = "a", *s2 = "b", *s3 = "c";
	assert(ec_buf_str_put(&b, s1));
	assert(ec_buf_str_put(&b, s2));
	assert(ec_buf_str_put(&b, s3));
	const char* out;
	assert(ec_buf_str_peek(&b, &out) && strcmp(out, "a") == 0);
	assert(ec_buf_str_get(&b, &out) && strcmp(out, "a") == 0);
	ec_buf_str_free(&b);
}

static void
test_peek_get_neg_empty(void)
{
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 1, NULL));
	const char* out;
	assert(!ec_buf_str_peek(&b, &out));
	assert(!ec_buf_str_get(&b, &out));
	ec_buf_str_free(&b);
}

static void
test_reserve_compact_rbuf_wbuf(void)
{
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 1, NULL));
	const char* s = "x";
	for (int i = 0; i < 3; i++)
		assert(ec_buf_str_put(&b, s));
	const char* out;
	assert(ec_buf_str_get(&b, &out));
	ec_buf_str_compact(&b);
	assert(ec_buf_str_rbuf(&b) != NULL);
	assert(ec_buf_str_wbuf(&b) != NULL);
	ec_buf_str_free(&b);
}

static void
test_wrap_pos(void)
{
	const char* data[2] = { "hello", "world" };
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 1, NULL));
	ec_buf_str_wrap(&b, (const char**)data, 2);
	const char* out;
	assert(ec_buf_str_get(&b, &out) && strcmp(out, "hello") == 0);
	assert(ec_buf_str_get(&b, &out) && strcmp(out, "world") == 0);
	ec_buf_str_free(&b);
}

static void
test_clear_pos(void)
{
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 2, NULL));
	const char* s = "test";
	assert(ec_buf_str_put(&b, s));
	assert(ec_buf_str_size(&b) == 1);
	ec_buf_str_clear(&b);
	assert(ec_buf_str_size(&b) == 0);
	assert(ec_buf_str_valid(&b));
	ec_buf_str_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_str_clear(NULL);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_str_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_str_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 2, NULL));
	b.limit = 3;
	const char* s = "test";
	assert(ec_buf_str_put(&b, s));
	assert(ec_buf_str_put(&b, s));
	bool ok = ec_buf_str_reserve(&b, 5);
	assert(!ok);
	assert(!ec_buf_str_valid(&b));
	ec_buf_str_free(&b);
}

static void
test_put_neg_limit(void)
{
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 1, NULL));
	b.limit = 2;
	const char* s = "test";
	assert(ec_buf_str_put(&b, s));   /* wpos=1, cap=1, expands to 2 (OK) */
	assert(ec_buf_str_put(&b, s));   /* wpos=2, cap=2, would expand to 4, but limit=2, fails */
	bool ok = ec_buf_str_put(&b, s); /* This should fail - would need cap=4, but limit=2 */
	assert(!ok);
	assert(!ec_buf_str_valid(&b));
	ec_buf_str_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_str_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_str_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_str_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 2, NULL));
	assert(ec_buf_str_valid(&b));
	const char* s = "test";
	assert(ec_buf_str_put(&b, s));
	assert(ec_buf_str_valid(&b));
	ec_buf_str_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_str_valid(NULL));
	ec_buf_str_t b;
	assert(ec_buf_str_init(&b, 1, NULL));
	const char* out;
	assert(!ec_buf_str_get(&b, &out));
	assert(!ec_buf_str_valid(&b));
	ec_buf_str_free(&b);
}

int
main(void)
{
	printf("Starting ec_buffer_str tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_peek_get_pos();
	test_put_neg_limit();
	test_peek_get_neg_empty();
	test_reserve_compact_rbuf_wbuf();
	test_reserve_neg();
	test_wrap_pos();
	test_clear_pos();
	test_clear_neg();
	test_quota_neg();
	test_size_neg();
	test_rbuf_neg();
	test_wbuf_neg();
	test_compact_neg();
	test_valid_pos();
	test_valid_neg();
	printf("All ec_buffer_str tests passed.\n");
	return 0;
}
