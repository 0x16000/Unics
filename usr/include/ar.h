#ifndef _AR_H
#define _AR_H

#include <stdint.h>

/* Archive file magic signature */
#define ARMAG    "!<arch>\n"    /* Magic string that identifies archive file */
#define SARMAG   8              /* Size of magic string */

/* Archive member header */
struct ar_hdr {
    char ar_name[16];     /* Member file name, `/` terminated */
    char ar_date[12];     /* Member file date, decimal */
    char ar_uid[6];       /* Member file user ID, decimal */
    char ar_gid[6];       /* Member file group ID, decimal */
    char ar_mode[8];      /* Member file mode, octal */
    char ar_size[10];     /* Member file size, decimal */
    char ar_fmag[2];      /* File magic signature */
};

/* Size of archive member header */
#define ARHDR_SIZE  sizeof(struct ar_hdr)

/* Archive member header magic */
#define ARFMAG "`\n"

/* Special archive member names */
#define AR_SYMBOL_TABLE    "/"        /* Symbol table member name */
#define AR_STRING_TABLE    "//"       /* String table member name */
#define AR_BSD_SYMBOL_TABLE "__.SYMDEF" /* BSD symbol table */

/* Archive member name processing macros */
#define AR_NAME_SIZE 16

/* Function-like macros for parsing header fields */
#define AR_NAME(hdr)     ((hdr)->ar_name)
#define AR_DATE(hdr)     ((hdr)->ar_date)
#define AR_UID(hdr)      ((hdr)->ar_uid)
#define AR_GID(hdr)      ((hdr)->ar_gid)
#define AR_MODE(hdr)     ((hdr)->ar_mode)
#define AR_SIZE(hdr)     ((hdr)->ar_size)
#define AR_FMAG(hdr)     ((hdr)->ar_fmag)

/* Utility functions for header validation */
static inline int ar_check_magic(const char *magic) {
    int i;
    for (i = 0; i < SARMAG; i++) {
        if (magic[i] != ARMAG[i]) {
            return 0;
        }
    }
    return 1;
}

static inline int ar_check_fmag(const struct ar_hdr *hdr) {
    return (hdr->ar_fmag[0] == ARFMAG[0] && hdr->ar_fmag[1] == ARFMAG[1]);
}

/* Convert string field to integer (handles space-padding) */
static inline long ar_atol(const char *field, int size) {
    char buf[32];
    int i, j = 0;
    
    /* Copy non-space characters */
    for (i = 0; i < size && j < 31; i++) {
        if (field[i] != ' ' && field[i] != '\0') {
            buf[j++] = field[i];
        }
    }
    buf[j] = '\0';
    
    /* Convert to long */
    long result = 0;
    for (i = 0; buf[i] != '\0'; i++) {
        if (buf[i] >= '0' && buf[i] <= '9') {
            result = result * 10 + (buf[i] - '0');
        } else {
            break; /* Stop at first non-digit */
        }
    }
    return result;
}

/* Convert octal string field to integer */
static inline long ar_atool(const char *field, int size) {
    char buf[32];
    int i, j = 0;
    
    /* Copy non-space characters */
    for (i = 0; i < size && j < 31; i++) {
        if (field[i] != ' ' && field[i] != '\0') {
            buf[j++] = field[i];
        }
    }
    buf[j] = '\0';
    
    /* Convert octal to long */
    long result = 0;
    for (i = 0; buf[i] != '\0'; i++) {
        if (buf[i] >= '0' && buf[i] <= '7') {
            result = result * 8 + (buf[i] - '0');
        } else {
            break; /* Stop at first non-octal digit */
        }
    }
    return result;
}

/* Extract values from archive header */
static inline long ar_get_size(const struct ar_hdr *hdr) {
    return ar_atol(hdr->ar_size, 10);
}

static inline long ar_get_date(const struct ar_hdr *hdr) {
    return ar_atol(hdr->ar_date, 12);
}

static inline long ar_get_uid(const struct ar_hdr *hdr) {
    return ar_atol(hdr->ar_uid, 6);
}

static inline long ar_get_gid(const struct ar_hdr *hdr) {
    return ar_atol(hdr->ar_gid, 6);
}

static inline long ar_get_mode(const struct ar_hdr *hdr) {
    return ar_atool(hdr->ar_mode, 8);
}

/* Check if member name matches */
static inline int ar_name_match(const struct ar_hdr *hdr, const char *name) {
    int i;
    int name_len = 0;
    
    /* Find length of provided name */
    while (name[name_len] != '\0') name_len++;
    
    /* Compare names */
    for (i = 0; i < AR_NAME_SIZE && i < name_len; i++) {
        if (hdr->ar_name[i] != name[i]) {
            return 0;
        }
    }
    
    /* Check if we reached end of archive name or found terminator */
    if (i < AR_NAME_SIZE) {
        return (hdr->ar_name[i] == '/' || hdr->ar_name[i] == ' ' || hdr->ar_name[i] == '\0');
    }
    
    return (i == name_len);
}

/* Archive format constants */
#define AR_MAX_NAME_LEN  255  /* Maximum filename length in extended format */

/* Error codes */
#define AR_OK           0
#define AR_ERR_MAGIC   -1     /* Invalid archive magic */
#define AR_ERR_HEADER  -2     /* Invalid member header */
#define AR_ERR_EOF     -3     /* Unexpected end of file */
#define AR_ERR_NAME    -4     /* Invalid member name */

#endif /* _AR_H */
