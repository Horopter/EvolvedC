#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 2, NULL));
	ec_buf_u32_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_u32_init(NULL, 2, NULL));
}

static void
test_put_growth(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 2, NULL));
	for (int i = 0; i < 5; i++)
		assert(ec_buf_u32_put(&b, (uint32_t)(i + 1)));
	ec_buf_u32_free(&b);
}

static void
test_peek_get_sequence(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 2, NULL));
	assert(ec_buf_u32_put(&b, (uint32_t)1));
	assert(ec_buf_u32_put(&b, (uint32_t)2));
	uint32_t v;
	assert(ec_buf_u32_peek(&b, &v) && v == (uint32_t)1);
	assert(ec_buf_u32_get(&b, &v) && v == (uint32_t)1);
	assert(ec_buf_u32_get(&b, &v) && v == (uint32_t)2);
	ec_buf_u32_free(&b);
}

static void
test_empty_neg(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 1, NULL));
	uint32_t v;
	assert(!ec_buf_u32_peek(&b, &v));
	assert(!ec_buf_u32_get(&b, &v));
	ec_buf_u32_free(&b);
}

static void
test_compact_then_get(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 2, NULL));
	assert(ec_buf_u32_put(&b, (uint32_t)1));
	assert(ec_buf_u32_put(&b, (uint32_t)2));
	uint32_t v;
	assert(ec_buf_u32_get(&b, &v) && v == (uint32_t)1);
	ec_buf_u32_compact(&b);
	assert(ec_buf_u32_put(&b, (uint32_t)3));
	assert(ec_buf_u32_get(&b, &v) && v == (uint32_t)2);
	assert(ec_buf_u32_get(&b, &v) && v == (uint32_t)3);
	ec_buf_u32_free(&b);
}

static void
test_wrap_pos(void)
{
	uint32_t data[2] = { 7U, 8U };
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 1, NULL));
	ec_buf_u32_wrap(&b, data, 2);
	uint32_t v;
	assert(ec_buf_u32_get(&b, &v) && v == 7U);
	assert(ec_buf_u32_get(&b, &v) && v == 8U);
	ec_buf_u32_free(&b);
}

static void
test_clear_pos(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 2, NULL));
	assert(ec_buf_u32_put(&b, (uint32_t)1));
	assert(ec_buf_u32_put(&b, (uint32_t)2));
	assert(ec_buf_u32_size(&b) == 2);
	ec_buf_u32_clear(&b);
	assert(ec_buf_u32_size(&b) == 0);
	assert(ec_buf_u32_valid(&b));
	ec_buf_u32_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_u32_clear(NULL);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_u32_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_u32_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 2, NULL));
	b.limit = 3;
	assert(ec_buf_u32_put(&b, (uint32_t)1));
	assert(ec_buf_u32_put(&b, (uint32_t)2));
	bool ok = ec_buf_u32_reserve(&b, 5); /* Exceeds limit */
	assert(!ok);
	assert(!ec_buf_u32_valid(&b));
	ec_buf_u32_free(&b);
}

static void
test_put_neg_limit(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 1, NULL));
	b.limit = 2;
	assert(ec_buf_u32_put(&b, (uint32_t)1));   /* wpos=1, cap=1, expands to 2 (OK) */
	assert(ec_buf_u32_put(&b, (uint32_t)2));   /* wpos=2, cap=2, would expand to 4, but limit=2, fails */
	bool ok = ec_buf_u32_put(&b, (uint32_t)3); /* This should fail - would need cap=4, but limit=2 */
	assert(!ok);
	assert(!ec_buf_u32_valid(&b));
	ec_buf_u32_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_u32_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_u32_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_u32_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_u32_t b;
	assert(ec_buf_u32_init(&b, 2, NULL));
	assert(ec_buf_u32_valid(&b));
	assert(ec_buf_u32_put(&b, (uint32_t)1));
	assert(ec_buf_u32_valid(&b));
	ec_buf_u32_free(&b);
}

static void
test_valid_neg(void)
{
	assert(!ec_buf_u32_valid(NULL));
}

int
main(void)
{
	printf("Starting ec_buffer_u32 tests...\n");
	test_init_pos();
	test_init_neg();
	test_put_growth();
	test_put_neg_limit();
	test_peek_get_sequence();
	test_empty_neg();
	test_compact_then_get();
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
	printf("All ec_buffer_u32 tests passed.\n");
	return 0;
}
