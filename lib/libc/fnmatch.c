#include <fnmatch.h>
#include <string.h>

#define EOS '\0'

static int
rangematch(const char *pattern, char test, int flags, const char **newp)
{
	int negate = 0, ok = 0;
	char c, c2;

	if (*pattern == '!' || *pattern == '^') {
		negate = 1;
		pattern++;
	}

	while ((c = *pattern++) != ']') {
		if (c == EOS)
			return -1;
		if (c == '\\' && !(flags & FNM_NOESCAPE))
			c = *pattern++;
		if (*pattern == '-' && pattern[1] != ']' && pattern[1] != EOS) {
			pattern++;
			c2 = *pattern++;
			if (c2 == '\\' && !(flags & FNM_NOESCAPE))
				c2 = *pattern++;
			if (c <= test && test <= c2)
				ok = 1;
		} else if (c == test) {
			ok = 1;
		}
	}
	*newp = pattern;
	return negate ? !ok : ok;
}

static int
match(const char *pattern, const char *string, int flags)
{
	char c;
	const char *s = string;

	for (; (c = *pattern) != EOS; pattern++) {
		switch (c) {
		case '?':
			if (*string == EOS)
				return FNM_NOMATCH;
			if ((flags & FNM_PATHNAME) && *string == '/')
				return FNM_NOMATCH;
			if ((flags & FNM_PERIOD) && *string == '.' &&
			    (string == s || ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return FNM_NOMATCH;
			string++;
			break;
		case '*':
			while (*pattern == '*')
				pattern++;
			if (*pattern == EOS)
				return ((flags & FNM_PATHNAME) && strchr(string, '/')) ?
				       FNM_NOMATCH : 0;
			while (*string != EOS) {
				if (!(flags & FNM_PATHNAME) || *string != '/') {
					if (match(pattern, string, flags) == 0)
						return 0;
				}
				string++;
			}
			return FNM_NOMATCH;
		case '[':
			if (*string == EOS)
				return FNM_NOMATCH;
			if ((flags & FNM_PATHNAME) && *string == '/')
				return FNM_NOMATCH;
			if ((flags & FNM_PERIOD) && *string == '.' &&
			    (string == s || ((flags & FNM_PATHNAME) && *(string - 1) == '/')))
				return FNM_NOMATCH;
			if (rangematch(pattern + 1, *string, flags, &pattern) == 0)
				return FNM_NOMATCH;
			string++;
			break;
		case '\\':
			if (!(flags & FNM_NOESCAPE) && pattern[1] != EOS)
				c = *++pattern;
			/* fallthrough */
		default:
			if (*string != c)
				return FNM_NOMATCH;
			string++;
			break;
		}
	}
	return (*string == EOS) ? 0 : FNM_NOMATCH;
}

int
fnmatch(const char *pattern, const char *string, int flags)
{
	if (!pattern || !string)
		return FNM_NOMATCH;
	return match(pattern, string, flags);
}
