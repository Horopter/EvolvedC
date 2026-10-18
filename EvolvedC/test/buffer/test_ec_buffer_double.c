#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 2, NULL));
	ec_buf_dbl_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_dbl_init(NULL, 2, NULL));
}

static void
test_put_pos(void)
{
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 2, NULL));
	assert(ec_buf_dbl_put(&b, 1.0));
	assert(ec_buf_dbl_put(&b, 2.0));
	assert(ec_buf_dbl_put(&b, 3.0)); /* reserve grows */
	ec_buf_dbl_free(&b);
}

static void
test_peek_get_pos(void)
{
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 2, NULL));
	assert(ec_buf_dbl_put(&b, 4.5));
	double v;
	assert(ec_buf_dbl_peek(&b, &v) && v == 4.5);
	assert(ec_buf_dbl_get(&b, &v) && v == 4.5);
	ec_buf_dbl_free(&b);
}

static void
test_peek_get_neg_empty(void)
{
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 2, NULL));
	double v;
	assert(!ec_buf_dbl_peek(&b, &v));
	assert(!ec_buf_dbl_get(&b, &v));
	ec_buf_dbl_free(&b);
}

static void
test_reserve_limit_neg_then_recover(void)
{
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 2, NULL));
	b.limit = 3; /* cap to 3 elements */
	assert(ec_buf_dbl_put(&b, 1.0));
	assert(ec_buf_dbl_put(&b, 2.0));
	bool ok = ec_buf_dbl_put(&b, 3.0); /* doubling to 4 exceeds limit -> fail */
	assert(!ok);
	assert(!ec_buf_dbl_valid(&b));
	ec_buf_dbl_clear(&b);
	b.limit = SIZE_MAX; /* remove limit */
	assert(ec_buf_dbl_reserve(&b, 5));
	assert(ec_buf_dbl_put(&b, 4.0));
	ec_buf_dbl_free(&b);
}

static void
test_compact_rbuf_wbuf(void)
{
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 4, NULL));
	for (int i = 0; i < 3; i++)
		assert(ec_buf_dbl_put(&b, (double)i));
	double v;
	assert(ec_buf_dbl_get(&b, &v));
	ec_buf_dbl_compact(&b);
	assert(ec_buf_dbl_rbuf(&b) != NULL);
	assert(ec_buf_dbl_wbuf(&b) != NULL);
	ec_buf_dbl_free(&b);
}

static void
test_wrap_pos(void)
{
	double data[2] = { 7.5, 8.5 };
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 1, NULL));
	ec_buf_dbl_wrap(&b, data, 2);
	double v;
	assert(ec_buf_dbl_get(&b, &v) && v == 7.5);
	assert(ec_buf_dbl_get(&b, &v) && v == 8.5);
	ec_buf_dbl_free(&b);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_dbl_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_dbl_size(NULL) == 0);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_dbl_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_dbl_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_dbl_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_dbl_t b;
	assert(ec_buf_dbl_init(&b, 2, NULL));
	assert(ec_buf_dbl_valid(&b));
	assert(ec_buf_dbl_put(&b, 1.0));
	assert(ec_buf_dbl_valid(&b));
	ec_buf_dbl_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_dbl_valid(NULL));
}

int
main(void)
{
	printf("Starting ec_buffer_double tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_pos();
	test_peek_get_pos();
	test_peek_get_neg_empty();
	test_reserve_limit_neg_then_recover();
	test_compact_rbuf_wbuf();
	test_wrap_pos();
	test_quota_neg();
	test_size_neg();
	test_rbuf_neg();
	test_wbuf_neg();
	test_compact_neg();
	test_valid_pos();
	test_valid_neg();
	printf("All ec_buffer_double tests passed.\n");
	return 0;
}
