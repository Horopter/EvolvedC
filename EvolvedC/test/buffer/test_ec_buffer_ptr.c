#include "../../src/ec_buffer.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

static void
test_init_pos(void)
{
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 2, NULL));
	ec_buf_ptr_free(&b);
}

static void
test_init_neg(void)
{
	assert(!ec_buf_ptr_init(NULL, 2, NULL));
}

static void
test_put_peek_get_pos(void)
{
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 2, NULL));
	int a = 1, bv = 2, cv = 3;
	void *p1 = &a, *p2 = &bv, *p3 = &cv;
	assert(ec_buf_ptr_put(&b, p1));
	assert(ec_buf_ptr_put(&b, p2));
	assert(ec_buf_ptr_put(&b, p3));
	void* out;
	assert(ec_buf_ptr_peek(&b, &out) && out == p1);
	assert(ec_buf_ptr_get(&b, &out) && out == p1);
	assert(ec_buf_ptr_get(&b, &out) && out == p2);
	assert(ec_buf_ptr_get(&b, &out) && out == p3);
	ec_buf_ptr_free(&b);
}

static void
test_peek_get_neg_empty(void)
{
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 1, NULL));
	void* out;
	assert(!ec_buf_ptr_peek(&b, &out));
	assert(!ec_buf_ptr_get(&b, &out));
	ec_buf_ptr_free(&b);
}

static void
test_reserve_compact_rbuf_wbuf(void)
{
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 1, NULL));
	int x = 1;
	void* p = &x;
	assert(ec_buf_ptr_put(&b, p));
	void* out;
	assert(ec_buf_ptr_get(&b, &out));
	ec_buf_ptr_compact(&b);
	assert(ec_buf_ptr_rbuf(&b) != NULL);
	assert(ec_buf_ptr_wbuf(&b) != NULL);
	ec_buf_ptr_free(&b);
}

static void
test_wrap_pos(void)
{
	int a = 1, bv = 2;
	void* data[2] = { &a, &bv };
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 1, NULL));
	ec_buf_ptr_wrap(&b, data, 2);
	void* out;
	assert(ec_buf_ptr_get(&b, &out) && out == &a);
	assert(ec_buf_ptr_get(&b, &out) && out == &bv);
	ec_buf_ptr_free(&b);
}

static void
test_clear_pos(void)
{
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 2, NULL));
	int x = 1;
	void* p = &x;
	assert(ec_buf_ptr_put(&b, p));
	assert(ec_buf_ptr_size(&b) == 1);
	ec_buf_ptr_clear(&b);
	assert(ec_buf_ptr_size(&b) == 0);
	assert(ec_buf_ptr_valid(&b));
	ec_buf_ptr_free(&b);
}

static void
test_clear_neg(void)
{
	ec_buf_ptr_clear(NULL);
}

static void
test_quota_neg(void)
{
	assert(ec_buf_ptr_quota(NULL) == 0);
}

static void
test_size_neg(void)
{
	assert(ec_buf_ptr_size(NULL) == 0);
}

static void
test_reserve_neg(void)
{
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 2, NULL));
	b.limit = 3;
	int x = 1, y = 2;
	void *p1 = &x, *p2 = &y;
	assert(ec_buf_ptr_put(&b, p1));
	assert(ec_buf_ptr_put(&b, p2));
	bool ok = ec_buf_ptr_reserve(&b, 5);
	assert(!ok);
	assert(!ec_buf_ptr_valid(&b));
	ec_buf_ptr_free(&b);
}

static void
test_put_neg_limit(void)
{
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 1, NULL));
	b.limit = 2;
	int x = 1, y = 2, z = 3;
	void *p1 = &x, *p2 = &y, *p3 = &z;
	assert(ec_buf_ptr_put(&b, p1));   /* wpos=1, cap=1, expands to 2 (OK) */
	assert(ec_buf_ptr_put(&b, p2));   /* wpos=2, cap=2, would expand to 4, but limit=2, fails */
	bool ok = ec_buf_ptr_put(&b, p3); /* This should fail - would need cap=4, but limit=2 */
	assert(!ok);
	assert(!ec_buf_ptr_valid(&b));
	ec_buf_ptr_free(&b);
}

static void
test_rbuf_neg(void)
{
	assert(ec_buf_ptr_rbuf(NULL) == NULL);
}

static void
test_wbuf_neg(void)
{
	assert(ec_buf_ptr_wbuf(NULL) == NULL);
}

static void
test_compact_neg(void)
{
	ec_buf_ptr_compact(NULL);
}

static void
test_valid_pos(void)
{
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 2, NULL));
	assert(ec_buf_ptr_valid(&b));
	int x = 1;
	void* p = &x;
	assert(ec_buf_ptr_put(&b, p));
	assert(ec_buf_ptr_valid(&b));
	ec_buf_ptr_free(&b);
}

static void
test_valid_flag_neg(void)
{
	assert(!ec_buf_ptr_valid(NULL));
	ec_buf_ptr_t b;
	assert(ec_buf_ptr_init(&b, 1, NULL));
	void* out;
	assert(!ec_buf_ptr_get(&b, &out));
	assert(!ec_buf_ptr_valid(&b));
	ec_buf_ptr_free(&b);
}

int
main(void)
{
	printf("Starting ec_buffer_ptr tests...\n");
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
	test_valid_flag_neg();
	printf("All ec_buffer_ptr tests passed.\n");
	return 0;
}
