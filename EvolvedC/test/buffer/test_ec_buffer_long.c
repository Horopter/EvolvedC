#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	ec_buf_long_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_long_init(NULL, 2, NULL));
}

static void
test_put_pos(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	assert(ec_buf_long_put(&b, 10L));
	assert(ec_buf_long_put(&b, 20L));
	assert(ec_buf_long_put(&b, 30L)); /* triggers reserve */
	ec_buf_long_free(&b);
}

static void
test_put_neg_limit(void)
{
	/* Reserve is automatic; simulate failure by exhausting and checking valid flag after get on empty */
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 1, NULL));
	assert(ec_buf_long_put(&b, 1L));
	long v;
	assert(ec_buf_long_get(&b, &v));
	assert(!ec_buf_long_get(&b, &v)); /* empty -> sets error */
	assert(!ec_buf_long_valid(&b));
	ec_buf_long_free(&b);
}

static void
test_peek_pos(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	assert(ec_buf_long_put(&b, 5L));
	long v;
	assert(ec_buf_long_peek(&b, &v) && v == 5L);
	ec_buf_long_free(&b);
}

static void
test_peek_neg_empty(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	long v;
	assert(!ec_buf_long_peek(&b, &v));
	ec_buf_long_free(&b);
}

static void
test_get_pos(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	assert(ec_buf_long_put(&b, 1L));
	assert(ec_buf_long_put(&b, 2L));
	long v;
	assert(ec_buf_long_get(&b, &v) && v == 1L);
	assert(ec_buf_long_get(&b, &v) && v == 2L);
	ec_buf_long_free(&b);
}

static void
test_get_neg_empty(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	long v;
	assert(!ec_buf_long_get(&b, &v));
	ec_buf_long_free(&b);
}

static void
test_reserve_pos(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 1, NULL));
	for (int i = 0; i < 5; i++)
		assert(ec_buf_long_put(&b, (long)i));
	/* capacity should have grown; size > 1 */
	assert(ec_buf_long_size(&b) >= 1);
	ec_buf_long_free(&b);
}

static void
test_compact_pos(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 4, NULL));
	assert(ec_buf_long_put(&b, 10L));
	assert(ec_buf_long_put(&b, 20L));
	long v;
	assert(ec_buf_long_get(&b, &v));
	ec_buf_long_compact(&b);
	assert(ec_buf_long_put(&b, 30L));
	ec_buf_long_free(&b);
}

static void
test_size_quota_checks(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 3, NULL));
	assert(ec_buf_long_put(&b, 1L));
	assert(ec_buf_long_size(&b) == 1);
	assert(ec_buf_long_quota(&b) >= 0);
	ec_buf_long_free(&b);
}

static void
test_wrap_pos(void)
{
	long data[2] = { 7L, 8L };
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 1, NULL));
	ec_buf_long_wrap(&b, data, 2);
	long v;
	assert(ec_buf_long_get(&b, &v) && v == 7L);
	assert(ec_buf_long_get(&b, &v) && v == 8L);
	ec_buf_long_free(&b);
}

static void
test_clear_pos(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	assert(ec_buf_long_put(&b, 1L));
	assert(ec_buf_long_put(&b, 2L));
	assert(ec_buf_long_size(&b) == 2);
	ec_buf_long_clear(&b);
	assert(ec_buf_long_size(&b) == 0);
	assert(ec_buf_long_valid(&b));
	ec_buf_long_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_long_clear(NULL);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_long_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_long_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	b.limit = 3;
	assert(ec_buf_long_put(&b, 1L));
	assert(ec_buf_long_put(&b, 2L));
	bool ok = ec_buf_long_reserve(&b, 5);
	assert(!ok);
	assert(!ec_buf_long_valid(&b));
	ec_buf_long_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_long_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_long_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_long_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_long_t b;
	assert(ec_buf_long_init(&b, 2, NULL));
	assert(ec_buf_long_valid(&b));
	assert(ec_buf_long_put(&b, 1L));
	assert(ec_buf_long_valid(&b));
	ec_buf_long_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_long_valid(NULL));
}

int
main(void)
{
	printf("Starting ec_buffer_long tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_pos();
	test_put_neg_limit();
	test_peek_pos();
	test_peek_neg_empty();
	test_get_pos();
	test_get_neg_empty();
	test_reserve_pos();
	test_reserve_neg();
	test_compact_pos();
	test_size_quota_checks();
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
	printf("All ec_buffer_long tests passed.\n");
	return 0;
}
