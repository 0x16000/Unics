#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <db.h>

/* Forward declarations of type-specific open functions */
static DB *btree_open(const char *fname, int flags, int mode, const void *openinfo);
static DB *hash_open(const char *fname, int flags, int mode, const void *openinfo);
static DB *recno_open(const char *fname, int flags, int mode, const void *openinfo);

/* Generic close stub */
static int
db_close(DB *db) {
    if (!db)
        return RET_ERROR;
    /* Free internal data if allocated */
    if (db->internal) {
        free(db->internal);
    }
    free(db);
    return RET_SUCCESS;
}

/* Stub implementations for DB methods returning error */
static int
db_del(const DB *db, const DBT *key, unsigned int flags) {
    (void)db; (void)key; (void)flags;
    return RET_ERROR; /* Not implemented */
}

static int
db_get(const DB *db, const DBT *key, DBT *data, unsigned int flags) {
    (void)db; (void)key; (void)data; (void)flags;
    return RET_ERROR; /* Not implemented */
}

static int
db_put(const DB *db, DBT *key, const DBT *data, unsigned int flags) {
    (void)db; (void)key; (void)data; (void)flags;
    return RET_ERROR; /* Not implemented */
}

static int
db_seq(const DB *db, DBT *key, DBT *data, unsigned int flags) {
    (void)db; (void)key; (void)data; (void)flags;
    return RET_ERROR; /* Not implemented */
}

static int
db_sync(const DB *db, unsigned int flags) {
    (void)db; (void)flags;
    return RET_ERROR; /* Not implemented */
}

static int
db_fd(const DB *db) {
    (void)db;
    return -1; /* Not implemented */
}

/*
 * dbopen: open a database of given DBTYPE, returns DB handle or NULL on error.
 */
DB *
dbopen(const char *fname, int flags, int mode, DBTYPE type, const void *openinfo)
{
    switch(type) {
    case DB_BTREE:
        return btree_open(fname, flags, mode, openinfo);
    case DB_HASH:
        return hash_open(fname, flags, mode, openinfo);
    case DB_RECNO:
        return recno_open(fname, flags, mode, openinfo);
    default:
        errno = EINVAL;
        return NULL;
    }
}

/*
 * Stub: btree_open just allocates DB and fills in method pointers
 */
static DB *
btree_open(const char *fname, int flags, int mode, const void *openinfo)
{
    (void)fname; (void)flags; (void)mode; (void)openinfo;

    DB *db = calloc(1, sizeof(DB));
    if (!db)
        return NULL;

    db->type = DB_BTREE;
    db->close = db_close;
    db->del = db_del;
    db->get = db_get;
    db->put = db_put;
    db->seq = db_seq;
    db->sync = db_sync;
    db->fd = db_fd;
    db->internal = NULL;  /* would point to btree-specific struct */

    return db;
}

/*
 * Stub: hash_open just allocates DB and fills in method pointers
 */
static DB *
hash_open(const char *fname, int flags, int mode, const void *openinfo)
{
    (void)fname; (void)flags; (void)mode; (void)openinfo;

    DB *db = calloc(1, sizeof(DB));
    if (!db)
        return NULL;

    db->type = DB_HASH;
    db->close = db_close;
    db->del = db_del;
    db->get = db_get;
    db->put = db_put;
    db->seq = db_seq;
    db->sync = db_sync;
    db->fd = db_fd;
    db->internal = NULL;  /* would point to hash-specific struct */

    return db;
}

/*
 * Stub: recno_open just allocates DB and fills in method pointers
 */
static DB *
recno_open(const char *fname, int flags, int mode, const void *openinfo)
{
    (void)fname; (void)flags; (void)mode; (void)openinfo;

    DB *db = calloc(1, sizeof(DB));
    if (!db)
        return NULL;

    db->type = DB_RECNO;
    db->close = db_close;
    db->del = db_del;
    db->get = db_get;
    db->put = db_put;
    db->seq = db_seq;
    db->sync = db_sync;
    db->fd = db_fd;
    db->internal = NULL;  /* would point to recno-specific struct */

    return db;
}
