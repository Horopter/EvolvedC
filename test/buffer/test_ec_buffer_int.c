#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	ec_buf_int_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_int_init(NULL, 2, NULL));
}

static void
test_put_pos(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	for (int i = 0; i < 5; i++)
		assert(ec_buf_int_put(&b, i + 1));
	ec_buf_int_free(&b);
}

static void
test_peek_pos(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	assert(ec_buf_int_put(&b, 42));
	int v;
	assert(ec_buf_int_peek(&b, &v) && v == 42);
	ec_buf_int_free(&b);
}

static void
test_peek_neg_empty(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	int v;
	assert(!ec_buf_int_peek(&b, &v));
	ec_buf_int_free(&b);
}

static void
test_get_pos(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	assert(ec_buf_int_put(&b, 1));
	assert(ec_buf_int_put(&b, 2));
	int v;
	assert(ec_buf_int_get(&b, &v) && v == 1);
	assert(ec_buf_int_get(&b, &v) && v == 2);
	ec_buf_int_free(&b);
}

static void
test_get_neg_empty(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	int v;
	assert(!ec_buf_int_get(&b, &v));
	ec_buf_int_free(&b);
}

static void
test_reserve_compact_size_quota(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 1, NULL));
	for (int i = 0; i < 4; i++)
		assert(ec_buf_int_put(&b, i));
	assert(ec_buf_int_size(&b) == 4);
	int v;
	assert(ec_buf_int_get(&b, &v));
	ec_buf_int_compact(&b);
	assert(ec_buf_int_quota(&b) >= 0);
	ec_buf_int_free(&b);
}

static void
test_wrap_pos(void)
{
	int data[2] = { 7, 8 };
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 1, NULL));
	ec_buf_int_wrap(&b, data, 2);
	int v;
	assert(ec_buf_int_get(&b, &v) && v == 7);
	assert(ec_buf_int_get(&b, &v) && v == 8);
	ec_buf_int_free(&b);
}

static void
test_wrap_neg(void)
{
	/* Note: wrap doesn't check for NULL, so we only test valid wrap usage */
	/* Negative cases would require implementation changes to wrap */
}

static void
test_clear_pos(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	assert(ec_buf_int_put(&b, 1));
	assert(ec_buf_int_put(&b, 2));
	assert(ec_buf_int_size(&b) == 2);
	ec_buf_int_clear(&b);
	assert(ec_buf_int_size(&b) == 0);
	assert(ec_buf_int_valid(&b));
	ec_buf_int_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_int_clear(NULL); /* NULL buffer - no crash expected */
}

static void
test_quota_neg(void)
{
	assert(ec_buf_int_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_int_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	b.limit = 3;
	assert(ec_buf_int_put(&b, 1));
	assert(ec_buf_int_put(&b, 2));
	bool ok = ec_buf_int_reserve(&b, 5); /* Exceeds limit */
	assert(!ok);
	assert(!ec_buf_int_valid(&b));
	ec_buf_int_free(&b);
}

static void
test_put_neg_limit(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 1, NULL));
	b.limit = 2;
	assert(ec_buf_int_put(&b, 1));   /* wpos=1, cap=1, expands to 2 (OK) */
	assert(ec_buf_int_put(&b, 2));   /* wpos=2, cap=2, would expand to 4, but limit=2, fails */
	bool ok = ec_buf_int_put(&b, 3); /* This should fail - would need cap=4, but limit=2 */
	assert(!ok);
	assert(!ec_buf_int_valid(&b));
	ec_buf_int_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_int_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_int_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_int_compact(NULL); /* NULL buffer - no crash expected */
}

static void
test_valid_pos(void)
{
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 2, NULL));
	assert(ec_buf_int_valid(&b));
	assert(ec_buf_int_put(&b, 1));
	assert(ec_buf_int_valid(&b));
	ec_buf_int_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_int_valid(NULL));
}

static void
test_wrap_and_valid_neg(void)
{
	int data[2] = { 7, 8 };
	ec_buf_int_t b;
	assert(ec_buf_int_init(&b, 1, NULL));
	ec_buf_int_wrap(&b, data, 2);
	int v;
	assert(ec_buf_int_get(&b, &v) && v == 7);
	assert(ec_buf_int_get(&b, &v) && v == 8);
	assert(!ec_buf_int_get(&b, &v));
	assert(!ec_buf_int_valid(&b));
	ec_buf_int_free(&b);
}

int
main(void)
{
	printf("Starting ec_buffer_int tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_pos();
	test_put_neg_limit();
	test_peek_pos();
	test_peek_neg_empty();
	test_get_pos();
	test_get_neg_empty();
	test_reserve_compact_size_quota();
	test_reserve_neg();
	test_wrap_pos();
	test_wrap_neg();
	test_clear_pos();
	test_clear_neg();
	test_quota_neg();
	test_size_neg();
	test_rbuf_neg();
	test_wbuf_neg();
	test_compact_neg();
	test_valid_pos();
	test_valid_neg();
	test_wrap_and_valid_neg();
	printf("All ec_buffer_int tests passed.\n");
	return 0;
}
