#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../src/ec_stack.h"

EC_STACK_DEF(char, ch)

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
test_stack_char_push_peek(void* mp)
{
	stamp(); printf("test_stack_char_push_peek\n");
	ec_stack_ch_t s; assert(ec_stack_ch_init(&s, mp) == EC_OK);
	char a = 'a', b = 'b';
	assert(s.push(&s, &a) == EC_OK);
	assert(s.push(&s, &b) == EC_OK);
	char* peek_val = malloc(sizeof(char));
	assert(peek_val);
	assert(s.peek(&s, (void**)&peek_val) == EC_OK && *peek_val == 'b');
	s.free_fn(&s);
	free(peek_val);
	printf("pass: test_stack_char_push_peek\n");
}

void
test_stack_char_pop(void* mp)
{
	stamp(); printf("test_stack_char_pop\n");
	ec_stack_ch_t s; assert(ec_stack_ch_init(&s, mp) == EC_OK);
	char a='x', b='y', c='z'; s.push(&s, &a); s.push(&s, &b); s.push(&s, &c);
	char* out = malloc(sizeof(char)); assert(out);
	assert(s.pop(&s, (void**)&out) == EC_OK && *out == 'z');
	assert(s.pop(&s, (void**)&out) == EC_OK && *out == 'y');
	assert(s.pop(&s, (void**)&out) == EC_OK && *out == 'x');
	assert(s.pop(&s, (void**)&out) == EC_ERR_OUT_OF_BOUNDS);
	s.free_fn(&s);
	free(out);
	printf("pass: test_stack_char_pop\n");
}

int
main(void)
{
	printf("Starting ec_stack_char tests...\n");
	void* mp = NULL;
	test_stack_char_push_peek(mp);
	test_stack_char_pop(mp);
	printf("All ec_stack_char tests passed.\n");
	return 0;
}
