#ifndef _ASR_H
#define _ASR_H

#include <stdint.h>

/* Archive Symbol Record structures and definitions */

/* Symbol table entry in archive */
struct asr_symbol {
    char *name;           /* Symbol name */
    uint32_t offset;      /* Offset to member containing symbol */
    uint32_t member_idx;  /* Index of member in archive */
    uint16_t type;        /* Symbol type flags */
    uint16_t binding;     /* Symbol binding (local, global, weak) */
};

/* Symbol table header (BSD format) */
struct asr_symtab_hdr {
    uint32_t magic;       /* Symbol table magic */
    uint32_t count;       /* Number of symbols */
    uint32_t str_size;    /* Size of string table */
    uint32_t reserved;    /* Reserved for future use */
};

/* Symbol table (System V format) */
struct asr_sysv_symtab {
    uint32_t count;       /* Number of symbols (big-endian) */
    /* Followed by count * uint32_t offsets */
    /* Followed by concatenated null-terminated symbol names */
};

/* Archive member index entry */
struct asr_member {
    char *name;           /* Member name */
    uint32_t offset;      /* Offset in archive file */
    uint32_t size;        /* Size of member data */
    uint32_t date;        /* Modification date */
    uint16_t uid;         /* User ID */
    uint16_t gid;         /* Group ID */
    uint16_t mode;        /* File mode */
    uint16_t flags;       /* Member flags */
};

/* Archive symbol record context */
struct asr_context {
    struct asr_symbol *symbols;    /* Symbol table */
    struct asr_member *members;    /* Member table */
    char *string_table;            /* String data */
    uint32_t symbol_count;         /* Number of symbols */
    uint32_t member_count;         /* Number of members */
    uint32_t string_size;          /* String table size */
    uint32_t format;               /* Archive format type */
};

/* Magic numbers */
#define ASR_MAGIC_BSD     0x21436855  /* BSD symbol table magic */
#define ASR_MAGIC_SYSV    0x213C6172  /* System V format magic */

/* Symbol types */
#define ASR_SYM_UNDEF     0x0000      /* Undefined symbol */
#define ASR_SYM_TEXT      0x0001      /* Text section symbol */
#define ASR_SYM_DATA      0x0002      /* Data section symbol */
#define ASR_SYM_BSS       0x0004      /* BSS section symbol */
#define ASR_SYM_FUNC      0x0008      /* Function symbol */
#define ASR_SYM_OBJECT    0x0010      /* Data object symbol */
#define ASR_SYM_COMMON    0x0020      /* Common symbol */

/* Symbol binding */
#define ASR_BIND_LOCAL    0x0000      /* Local symbol */
#define ASR_BIND_GLOBAL   0x0001      /* Global symbol */
#define ASR_BIND_WEAK     0x0002      /* Weak symbol */

/* Archive formats */
#define ASR_FORMAT_BSD    0x0001      /* BSD archive format */
#define ASR_FORMAT_SYSV   0x0002      /* System V archive format */
#define ASR_FORMAT_GNU    0x0004      /* GNU archive format */

/* Member flags */
#define ASR_MEMBER_SYMTAB 0x0001      /* Member is symbol table */
#define ASR_MEMBER_STRTAB 0x0002      /* Member is string table */
#define ASR_MEMBER_REGULAR 0x0004     /* Regular archive member */

/* Error codes */
#define ASR_OK            0
#define ASR_ERR_NOMEM    -1           /* Out of memory */
#define ASR_ERR_FORMAT   -2           /* Invalid format */
#define ASR_ERR_CORRUPT  -3           /* Corrupted data */
#define ASR_ERR_NOTFOUND -4           /* Symbol not found */
#define ASR_ERR_IO       -5           /* I/O error */

/* Function prototypes for ASR operations */

/* Initialize ASR context */
static inline void asr_init_context(struct asr_context *ctx) {
    ctx->symbols = (void*)0;
    ctx->members = (void*)0;
    ctx->string_table = (void*)0;
    ctx->symbol_count = 0;
    ctx->member_count = 0;
    ctx->string_size = 0;
    ctx->format = 0;
}

/* Byte order conversion for different endianness */
static inline uint32_t asr_swap32(uint32_t val) {
    return ((val & 0xFF000000) >> 24) |
           ((val & 0x00FF0000) >> 8)  |
           ((val & 0x0000FF00) << 8)  |
           ((val & 0x000000FF) << 24);
}

static inline uint16_t asr_swap16(uint16_t val) {
    return ((val & 0xFF00) >> 8) | ((val & 0x00FF) << 8);
}

/* Convert big-endian to host byte order */
static inline uint32_t asr_be32toh(uint32_t val) {
    /* Assume little-endian host for OS dev */
    return asr_swap32(val);
}

static inline uint16_t asr_be16toh(uint16_t val) {
    return asr_swap16(val);
}

/* Hash function for symbol names */
static inline uint32_t asr_hash_symbol(const char *name) {
    uint32_t hash = 5381;
    int c;
    
    while ((c = *name++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

/* String comparison */
static inline int asr_strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

/* Find symbol in symbol table */
static inline struct asr_symbol* asr_find_symbol(struct asr_context *ctx, const char *name) {
    uint32_t i;
    
    if (!ctx || !ctx->symbols || !name) {
        return (void*)0;
    }
    
    for (i = 0; i < ctx->symbol_count; i++) {
        if (ctx->symbols[i].name && asr_strcmp(ctx->symbols[i].name, name) == 0) {
            return &ctx->symbols[i];
        }
    }
    
    return (void*)0;
}

/* Get symbol name from string table */
static inline const char* asr_get_symbol_name(struct asr_context *ctx, uint32_t str_offset) {
    if (!ctx || !ctx->string_table || str_offset >= ctx->string_size) {
        return (void*)0;
    }
    return ctx->string_table + str_offset;
}

/* Check if symbol is defined */
static inline int asr_is_symbol_defined(const struct asr_symbol *sym) {
    return sym && (sym->type != ASR_SYM_UNDEF);
}

/* Check if symbol is global */
static inline int asr_is_symbol_global(const struct asr_symbol *sym) {
    return sym && (sym->binding == ASR_BIND_GLOBAL);
}

/* Check if symbol is function */
static inline int asr_is_symbol_function(const struct asr_symbol *sym) {
    return sym && (sym->type & ASR_SYM_FUNC);
}

/* Check if symbol is data object */
static inline int asr_is_symbol_object(const struct asr_symbol *sym) {
    return sym && (sym->type & ASR_SYM_OBJECT);
}

/* Get member by index */
static inline struct asr_member* asr_get_member(struct asr_context *ctx, uint32_t index) {
    if (!ctx || !ctx->members || index >= ctx->member_count) {
        return (void*)0;
    }
    return &ctx->members[index];
}

/* Find member by name */
static inline struct asr_member* asr_find_member(struct asr_context *ctx, const char *name) {
    uint32_t i;
    
    if (!ctx || !ctx->members || !name) {
        return (void*)0;
    }
    
    for (i = 0; i < ctx->member_count; i++) {
        if (ctx->members[i].name && asr_strcmp(ctx->members[i].name, name) == 0) {
            return &ctx->members[i];
        }
    }
    
    return (void*)0;
}

/* Symbol table iteration */
#define ASR_FOR_EACH_SYMBOL(ctx, sym, i) \
    for ((i) = 0, (sym) = (ctx)->symbols; \
         (i) < (ctx)->symbol_count; \
         (i)++, (sym)++)

/* Member table iteration */
#define ASR_FOR_EACH_MEMBER(ctx, member, i) \
    for ((i) = 0, (member) = (ctx)->members; \
         (i) < (ctx)->member_count; \
         (i)++, (member)++)

/* Utility macros */
#define ASR_ALIGN(x, a)       (((x) + (a) - 1) & ~((a) - 1))
#define ASR_ALIGN_4(x)        ASR_ALIGN(x, 4)
#define ASR_ALIGN_8(x)        ASR_ALIGN(x, 8)

/* Symbol table sizes */
#define ASR_MAX_SYMBOLS      65536    /* Maximum symbols per archive */
#define ASR_MAX_MEMBERS      4096     /* Maximum members per archive */
#define ASR_MAX_STRING_SIZE  (1024*1024)  /* Maximum string table size */

#endif /* _ASR_H */
