#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 2, NULL));
	ec_buf_ull_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_ull_init(NULL, 2, NULL));
}

static void
test_put_growth(void)
{
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 2, NULL));
	assert(ec_buf_ull_put(&b, 7ULL));
	assert(ec_buf_ull_put(&b, 8ULL));
	assert(ec_buf_ull_put(&b, 9ULL));
	ec_buf_ull_free(&b);
}

static void
test_peek_get_sequence(void)
{
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 2, NULL));
	assert(ec_buf_ull_put(&b, 7ULL));
	assert(ec_buf_ull_put(&b, 8ULL));
	unsigned long long v;
	assert(ec_buf_ull_peek(&b, &v) && v == 7ULL);
	assert(ec_buf_ull_get(&b, &v) && v == 7ULL);
	assert(ec_buf_ull_get(&b, &v) && v == 8ULL);
	ec_buf_ull_free(&b);
}

static void
test_empty_neg_and_compact(void)
{
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 2, NULL));
	unsigned long long v;
	assert(!ec_buf_ull_peek(&b, &v));
	assert(!ec_buf_ull_get(&b, &v));
	assert(ec_buf_ull_put(&b, 9ULL));
	ec_buf_ull_compact(&b);
	assert(ec_buf_ull_get(&b, &v) && v == 9ULL);
	ec_buf_ull_free(&b);
}

static void
test_wrap_pos(void)
{
	unsigned long long data[2] = { 7ULL, 8ULL };
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 1, NULL));
	ec_buf_ull_wrap(&b, data, 2);
	unsigned long long v;
	assert(ec_buf_ull_get(&b, &v) && v == 7ULL);
	assert(ec_buf_ull_get(&b, &v) && v == 8ULL);
	ec_buf_ull_free(&b);
}

static void
test_clear_pos(void)
{
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 2, NULL));
	assert(ec_buf_ull_put(&b, 7ULL));
	assert(ec_buf_ull_put(&b, 8ULL));
	assert(ec_buf_ull_size(&b) == 2);
	ec_buf_ull_clear(&b);
	assert(ec_buf_ull_size(&b) == 0);
	assert(ec_buf_ull_valid(&b));
	ec_buf_ull_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_ull_clear(NULL);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_ull_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_ull_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 2, NULL));
	b.limit = 3;
	assert(ec_buf_ull_put(&b, 7ULL));
	assert(ec_buf_ull_put(&b, 8ULL));
	bool ok = ec_buf_ull_reserve(&b, 5);
	assert(!ok);
	assert(!ec_buf_ull_valid(&b));
	ec_buf_ull_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_ull_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_ull_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_ull_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 2, NULL));
	assert(ec_buf_ull_valid(&b));
	assert(ec_buf_ull_put(&b, 7ULL));
	assert(ec_buf_ull_valid(&b));
	ec_buf_ull_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_ull_valid(NULL));
}

static void
test_put_neg_limit(void)
{
	ec_buf_ull_t b;
	assert(ec_buf_ull_init(&b, 1, NULL));
	b.limit = 2;
	assert(ec_buf_ull_put(&b, 7ULL));   /* wpos=1, cap=1, expands to 2 (OK) */
	assert(ec_buf_ull_put(&b, 8ULL));   /* wpos=2, cap=2, would expand to 4, but limit=2, fails */
	bool ok = ec_buf_ull_put(&b, 9ULL); /* This should fail - would need cap=4, but limit=2 */
	assert(!ok);
	assert(!ec_buf_ull_valid(&b));
	ec_buf_ull_free(&b);
}

int
main(void)
{
	printf("Starting ec_buffer_ull tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_growth();
	test_put_neg_limit();
	test_peek_get_sequence();
	test_empty_neg_and_compact();
	test_wrap_pos();
	test_clear_pos();
	test_clear_neg();
	test_quota_neg();
	test_size_neg();
	test_reserve_neg();
	test_rbuf_neg();
	test_wbuf_neg();
	test_compact_neg();
	test_valid_pos();
	test_valid_neg();
	printf("All ec_buffer_ull tests passed.\n");
	return 0;
}
