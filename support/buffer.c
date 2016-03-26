#include "buffer.h"

caryll_buffer *bufnew() {
	caryll_buffer *buf = malloc(sizeof(caryll_buffer));
	buf->s = sdsempty();
	buf->cursor = 0;
	return buf;
}
void buffree(caryll_buffer *buf) {
	if (!buf) return;
	if (buf->s) sdsfree(buf->s);
	free(buf);
}
size_t buflen(caryll_buffer *buf) {
	return sdslen(buf->s);
}
size_t bufpos(caryll_buffer *buf) {
	return buf->cursor;
}
void bufseek(caryll_buffer *buf, size_t pos) {
	buf->cursor = pos;
}

static void bufbeforewrite(caryll_buffer *buf, size_t towrite) {
	size_t curlen = sdslen(buf->s);
	size_t len = buf->cursor + towrite;
	if (len <= curlen) return;
	buf->s = sdsMakeRoomFor(buf->s, len - curlen);
	if (buf->s == NULL) return;
	if (len - curlen + 1 - towrite > 0) {
		memset(buf->s + curlen, 0, (len - curlen + 1 - towrite));
	}
	sdssetlen(buf->s, len);
	return;
}
void bufwrite8(caryll_buffer *buf, uint8_t byte) {
	bufbeforewrite(buf, 1);
	buf->s[buf->cursor++] = byte;
}
void bufwrite16l(caryll_buffer *buf, uint16_t x) {
	bufbeforewrite(buf, 2);
	buf->s[buf->cursor++] = x & 0xFF;
	buf->s[buf->cursor++] = (x >> 8) & 0xFF;
}
void bufwrite16b(caryll_buffer *buf, uint16_t x) {
	bufbeforewrite(buf, 2);
	buf->s[buf->cursor++] = (x >> 8) & 0xFF;
	buf->s[buf->cursor++] = x & 0xFF;
}
void bufwrite32l(caryll_buffer *buf, uint32_t x) {
	bufbeforewrite(buf, 4);
	buf->s[buf->cursor++] = x & 0xFF;
	buf->s[buf->cursor++] = (x >> 8) & 0xFF;
	buf->s[buf->cursor++] = (x >> 16) & 0xFF;
	buf->s[buf->cursor++] = (x >> 24) & 0xFF;
}
void bufwrite32b(caryll_buffer *buf, uint32_t x) {
	bufbeforewrite(buf, 4);
	buf->s[buf->cursor++] = (x >> 24) & 0xFF;
	buf->s[buf->cursor++] = (x >> 16) & 0xFF;
	buf->s[buf->cursor++] = (x >> 8) & 0xFF;
	buf->s[buf->cursor++] = x & 0xFF;
}
void bufwrite64l(caryll_buffer *buf, uint64_t x) {
	bufbeforewrite(buf, 8);
	buf->s[buf->cursor++] = x & 0xFF;
	buf->s[buf->cursor++] = (x >> 8) & 0xFF;
	buf->s[buf->cursor++] = (x >> 16) & 0xFF;
	buf->s[buf->cursor++] = (x >> 24) & 0xFF;
	buf->s[buf->cursor++] = (x >> 32) & 0xFF;
	buf->s[buf->cursor++] = (x >> 40) & 0xFF;
	buf->s[buf->cursor++] = (x >> 48) & 0xFF;
	buf->s[buf->cursor++] = (x >> 56) & 0xFF;
}
void bufwrite64b(caryll_buffer *buf, uint64_t x) {
	bufbeforewrite(buf, 8);
	buf->s[buf->cursor++] = (x >> 56) & 0xFF;
	buf->s[buf->cursor++] = (x >> 48) & 0xFF;
	buf->s[buf->cursor++] = (x >> 40) & 0xFF;
	buf->s[buf->cursor++] = (x >> 32) & 0xFF;
	buf->s[buf->cursor++] = (x >> 24) & 0xFF;
	buf->s[buf->cursor++] = (x >> 16) & 0xFF;
	buf->s[buf->cursor++] = (x >> 8) & 0xFF;
	buf->s[buf->cursor++] = x & 0xFF;
}
void bufwrite_sds(caryll_buffer *buf, sds str) {
	size_t len = sdslen(str);
	bufbeforewrite(buf, len);
	memcpy(buf->s + buf->cursor, str, len);
	buf->cursor += len;
}
void bufwrite_str(caryll_buffer *buf, const char *str) {
	size_t len = strlen(str);
	bufbeforewrite(buf, len);
	memcpy(buf->s + buf->cursor, str, len);
	buf->cursor += len;
}
void bufwrite_buf(caryll_buffer *buf, caryll_buffer *that) {
	size_t len = buflen(that);
	bufbeforewrite(buf, len);
	memcpy(buf->s + buf->cursor, that->s, len);
	buf->cursor += len;
}
