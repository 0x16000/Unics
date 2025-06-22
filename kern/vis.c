#include <sys/cdefs.h>
#include <stddef.h>
#include <string.h>
#include <sys/ctype.h>
#include <sys/types.h>
#include <vis.h>

static const char *ctl[] = {
	"^^", "^A", "^B", "^C", "^D", "^E", "^F", "^G",
	"^H", "^I", "^J", "^K", "^L", "^M", "^N", "^O",
	"^P", "^Q", "^R", "^S", "^T", "^U", "^V", "^W",
	"^X", "^Y", "^Z", "^[", "^\\", "^]", "^^", "^_"
};

/*
 * vis: encode character c into dst according to flags.
 * Returns pointer to next free position in dst.
 */
char *
vis(char *dst, int c, int flag, int nextc)
{
	char *start = dst;

	/* If VIS_NOSLASH is not set, output '\' before encoded char */
	if (!(flag & VIS_NOSLASH)) {
		switch (c) {
		case '\n':
			if (flag & VIS_NL) break;
			*dst++ = '\\';
			*dst++ = 'n';
			return dst;
		case '\r':
			*dst++ = '\\';
			*dst++ = 'r';
			return dst;
		case '\b':
			*dst++ = '\\';
			*dst++ = 'b';
			return dst;
		case '\a':
			*dst++ = '\\';
			*dst++ = 'a';
			return dst;
		case '\v':
			*dst++ = '\\';
			*dst++ = 'v';
			return dst;
		case '\t':
			if (flag & VIS_TAB) break;
			*dst++ = '\\';
			*dst++ = 't';
			return dst;
		case '\f':
			*dst++ = '\\';
			*dst++ = 'f';
			return dst;
		case ' ':
			if (flag & VIS_SP) {
				*dst++ = '\\';
				*dst++ = 's';
				return dst;
			}
			break;
		case '"':
			if (flag & VIS_DQ) {
				*dst++ = '\\';
				*dst++ = '"';
				return dst;
			}
			break;
		case '\\':
			*dst++ = '\\';
			*dst++ = '\\';
			return dst;
		default:
			break;
		}
	}

	/* Check if character is printable ASCII */
	if (!(flag & VIS_ALL) && isprint(c) &&
	    !(flag & VIS_WHITE && (c == ' ' || c == '\t' || c == '\n'))) {
		*dst++ = c;
		return dst;
	}

	/* Handle unsafe or non-printable characters */
	if (flag & VIS_CSTYLE) {
		if (c < 0x20 || c == 0x7f) {
			const char *ctlstr;
			if (c == 0x7f) {
				*dst++ = '\\';
				*dst++ = '?';
				return dst;
			}
			ctlstr = ctl[c];
			*dst++ = '\\';
			*dst++ = ctlstr[1];
			return dst;
		}
	}

	if (flag & VIS_OCTAL) {
		*dst++ = '\\';
		*dst++ = ((c >> 6) & 7) + '0';
		*dst++ = ((c >> 3) & 7) + '0';
		*dst++ = (c & 7) + '0';
		return dst;
	}

	/* Default: use octal if above didn't match */
	*dst++ = '\\';
	*dst++ = ((c >> 6) & 7) + '0';
	*dst++ = ((c >> 3) & 7) + '0';
	*dst++ = (c & 7) + '0';
	return dst;
}

/*
 * strvis: encode string src into dst using vis()
 * Returns length of encoded string.
 */
int
strvis(char *dst, const char *src, int flag)
{
	char *start = dst;

	while (*src) {
		dst = vis(dst, (unsigned char)*src++, flag, *src);
	}
	*dst = '\0';

	return (int)(dst - start);
}

/*
 * stravis: allocate and return a newly malloced encoded string of src
 */
#include <stdlib.h>

int
stravis(char **dst, const char *src, int flag)
{
	int len = strlen(src);
	/* max length: each char encoded as 4 + '\0' */
	int maxlen = len * 4 + 1;

	*dst = malloc(maxlen);
	if (*dst == NULL)
		return -1;

	strvis(*dst, src, flag);
	return 0;
}

/*
 * strnvis: encode up to n bytes of src into dst, null terminated.
 */
int
strnvis(char *dst, const char *src, size_t n, int flag)
{
	char *start = dst;
	size_t i = 0;

	while (i < n && src[i]) {
		dst = vis(dst, (unsigned char)src[i], flag, (i + 1 < n) ? src[i+1] : 0);
		i++;
	}
	*dst = '\0';
	return (int)(dst - start);
}

/*
 * strvisx: encode exactly n bytes from src into dst (for binary data)
 */
int
strvisx(char *dst, const char *src, size_t n, int flag)
{
	char *start = dst;
	size_t i;

	for (i = 0; i < n; i++) {
		dst = vis(dst, (unsigned char)src[i], flag, (i + 1 < n) ? src[i+1] : 0);
	}
	*dst = '\0';
	return (int)(dst - start);
}

/*
 * unvis and strunvis are more complex and normally implemented separately.
 * For brevity, we won't provide full unvis implementation here,
 * but here's a simple stub that just copies input to output.
 */

int
strunvis(char *dst, const char *src)
{
	while (*src) {
		*dst++ = *src++;
	}
	*dst = '\0';
	return 0;
}

int
unvis(char *cp, char c, int *astate, int flags)
{
	/* Simple stub, no real decoding */
	*cp = c;
	return UNVIS_VALID;
}

ssize_t
strnunvis(char *dst, const char *src, size_t n)
{
	size_t i;
	for (i = 0; i < n && src[i]; i++) {
		dst[i] = src[i];
	}
	dst[i] = '\0';
	return (ssize_t)i;
}
