#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 2, NULL));
	ec_buf_flt_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_flt_init(NULL, 2, NULL));
}

static void
test_put_pos(void)
{
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 2, NULL));
	assert(ec_buf_flt_put(&b, 1.0f));
	assert(ec_buf_flt_put(&b, 2.5f));
	assert(ec_buf_flt_put(&b, 3.5f)); /* triggers reserve growth */
	ec_buf_flt_free(&b);
}

static void
test_peek_get_pos(void)
{
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 2, NULL));
	assert(ec_buf_flt_put(&b, 4.5f));
	float v;
	assert(ec_buf_flt_peek(&b, &v) && v == 4.5f);
	assert(ec_buf_flt_get(&b, &v) && v == 4.5f);
	ec_buf_flt_free(&b);
}

static void
test_peek_get_neg_empty(void)
{
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 2, NULL));
	float v;
	assert(!ec_buf_flt_peek(&b, &v));
	assert(!ec_buf_flt_get(&b, &v));
	ec_buf_flt_free(&b);
}

static void
test_reserve_limit_neg(void)
{
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 2, NULL));
	b.limit = 3;
	assert(ec_buf_flt_put(&b, 1.0f));
	assert(ec_buf_flt_put(&b, 2.0f));
	bool ok = ec_buf_flt_put(&b, 3.0f); /* doubling to 4 exceeds limit -> fail */
	assert(!ok);
	assert(!ec_buf_flt_valid(&b));
	ec_buf_flt_free(&b);
}

static void
test_limit_recover_and_reserve_pos(void)
{
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 2, NULL));
	b.limit = 3;
	assert(ec_buf_flt_put(&b, 1.0f));
	assert(ec_buf_flt_put(&b, 2.0f));
	(void)ec_buf_flt_put(&b, 3.0f);
	ec_buf_flt_clear(&b);
	b.limit = SIZE_MAX;
	assert(ec_buf_flt_reserve(&b, 5));
	assert(ec_buf_flt_put(&b, 4.0f));
	ec_buf_flt_free(&b);
}

static void
test_compact_rbuf_wbuf_and_sizes(void)
{
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 4, NULL));
	for (int i = 0; i < 3; i++)
		assert(ec_buf_flt_put(&b, (float)i));
	assert(ec_buf_flt_size(&b) == 3);
	float v;
	assert(ec_buf_flt_get(&b, &v) && v == 0.0f);
	ec_buf_flt_compact(&b);
	assert(ec_buf_flt_size(&b) == 2);
	assert(ec_buf_flt_rbuf(&b) != NULL);
	assert(ec_buf_flt_wbuf(&b) != NULL);
	assert(ec_buf_flt_quota(&b) >= 0);
	ec_buf_flt_free(&b);
}

static void
test_wrap_pos(void)
{
	float data[2] = { 7.0f, 8.0f };
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 1, NULL));
	ec_buf_flt_wrap(&b, data, 2);
	float v;
	assert(ec_buf_flt_get(&b, &v) && v == 7.0f);
	assert(ec_buf_flt_get(&b, &v) && v == 8.0f);
	ec_buf_flt_free(&b);
}

static void
test_wrap_and_valid_neg(void)
{
	float data[2] = { 7.0f, 8.0f };
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 1, NULL));
	ec_buf_flt_wrap(&b, data, 2);
	float v;
	assert(ec_buf_flt_get(&b, &v) && v == 7.0f);
	assert(ec_buf_flt_get(&b, &v) && v == 8.0f);
	assert(!ec_buf_flt_get(&b, &v));
	assert(!ec_buf_flt_valid(&b));
	ec_buf_flt_free(&b);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_flt_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_flt_size(NULL) == 0);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_flt_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_flt_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_flt_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_flt_t b;
	assert(ec_buf_flt_init(&b, 2, NULL));
	assert(ec_buf_flt_valid(&b));
	assert(ec_buf_flt_put(&b, 1.0f));
	assert(ec_buf_flt_valid(&b));
	ec_buf_flt_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_flt_valid(NULL));
}

int
main(void)
{
	printf("Starting ec_buffer_flt tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_pos();
	test_peek_get_pos();
	test_peek_get_neg_empty();
	test_reserve_limit_neg();
	test_limit_recover_and_reserve_pos();
	test_compact_rbuf_wbuf_and_sizes();
	test_wrap_pos();
	test_wrap_and_valid_neg();
	test_quota_neg();
	test_size_neg();
	test_rbuf_neg();
	test_wbuf_neg();
	test_compact_neg();
	test_valid_pos();
	test_valid_neg();
	printf("All ec_buffer_flt tests passed.\n");
	return 0;
}
