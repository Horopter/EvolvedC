#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/ec_queue.h"

EC_QUEUE_DEF(char, ch)

static void
stamp(void)
{
	time_t t = time(NULL);
	struct tm* m = localtime(&t);
	if (m)
		{
			char b[64];
			strftime(b, sizeof(b), "[%Y-%m-%d %H:%M:%S]", m);
			printf("%s ", b);
		}
	else
		printf("[timeerr] ");
}

void
test_queue_char_enqueue_peek(void* mp)
{
	stamp(); printf("test_queue_char_enqueue_peek\n");
	ec_queue_ch_t q; assert(ec_queue_ch_init(&q, mp) == EC_OK);
	char a='a', b='b';
	assert(q.enqueue(&q, &a) == EC_OK);
	assert(q.enqueue(&q, &b) == EC_OK);
	char* peek_val = malloc(sizeof(char)); assert(peek_val);
	assert(q.peek(&q, (void**)&peek_val) == EC_OK && *peek_val == 'a');
	q.free_fn(&q);
	free(peek_val);
	printf("pass: test_queue_char_enqueue_peek\n");
}

void
test_queue_char_dequeue(void* mp)
{
	stamp(); printf("test_queue_char_dequeue\n");
	ec_queue_ch_t q; assert(ec_queue_ch_init(&q, mp) == EC_OK);
	char a='x', b='y', c='z'; q.enqueue(&q, &a); q.enqueue(&q, &b); q.enqueue(&q, &c);
	char* out = malloc(sizeof(char)); assert(out);
	assert(q.dequeue(&q, (void**)&out) == EC_OK && *out == 'x');
	assert(q.dequeue(&q, (void**)&out) == EC_OK && *out == 'y');
	assert(q.dequeue(&q, (void**)&out) == EC_OK && *out == 'z');
	assert(q.dequeue(&q, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	q.free_fn(&q);
	free(out);
	printf("pass: test_queue_char_dequeue\n");
}

int
main(void)
{
	printf("Starting ec_queue_char tests...\n");
	void* mp = NULL;
	test_queue_char_enqueue_peek(mp);
	test_queue_char_dequeue(mp);
	printf("All ec_queue_char tests passed.\n");
	return 0;
}
