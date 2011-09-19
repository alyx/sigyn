/*
 * libmowgli: A collection of useful routines for programming.
 * mowgli_string.c: Immutable string buffers with cheap manipulation.
 *
 * Copyright (c) 2007 William Pitcock <nenolod -at- sacredspiral.co.uk>
 * Copyright (c) 2007 Pippijn van Steenhoven <pippijn -at- one09.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice is present in all copies.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "mowgli.h"

mowgli_string_t *mowgli_string_create(void)
{
	mowgli_string_t *self = mowgli_alloc(sizeof(mowgli_string_t));

	self->size = 64;
	self->pos = 0;
	self->str = mowgli_alloc(self->size);

	self->append = &mowgli_string_append;
	self->append_char = &mowgli_string_append_char;
	self->reset = &mowgli_string_reset;
	self->destroy = &mowgli_string_destroy;

	return self;
}

void mowgli_string_reset(mowgli_string_t *self)
{
	return_if_fail(self != NULL);

	self->str[0] = self->pos = 0;
}

void mowgli_string_destroy(mowgli_string_t *self)
{
	return_if_fail(self != NULL);

	mowgli_free(self->str);
	mowgli_free(self);
}

void mowgli_string_append(mowgli_string_t *self, const char *src, size_t n)
{
	if (self->size - self->pos <= n)
	{
		char *new;

		self->size = MAX(self->size * 2, self->pos + n + 8);
		new = realloc(self->str, self->size);
		self->str = new;
	}

	memcpy(self->str + self->pos, src, n);
	self->pos += n;
	self->str[self->pos] = 0;
}

void mowgli_string_append_char(mowgli_string_t *self, const char c)
{
	if (self->size - self->pos <= 1)
	{
		char *new;

		self->size = MAX(self->size * 2, self->pos + 9);
		new = realloc(self->str, self->size);
		self->str = new;
	}

	self->str[self->pos++] = c;
	self->str[self->pos] = 0;
}

/* These functions are taken from Linux. */
size_t mowgli_strlcat(char *dest, const char *src, size_t count)
{
	size_t dsize = strlen(dest);
	size_t len = strlen(src);
	size_t res = dsize + len;

	dest += dsize;
	count -= dsize;

	if (len >= count)
		len = count - 1;

	memcpy(dest, src, len);

	dest[len] = 0;

	return res;
}

size_t mowgli_strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = strlen(src);

	if (size)
	{
		size_t len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}

	return ret;
}
