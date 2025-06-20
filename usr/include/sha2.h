#ifndef SHA2_H
#define SHA2_H

#include <stddef.h>  // for size_t
#include <stdint.h>  // for uint32_t, uint8_t
#include <stdbool.h> // for bool

#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32

typedef struct {
    uint32_t state[8];
    uint64_t bitlen;
    uint8_t data[SHA256_BLOCK_SIZE];
    size_t datalen;
} sha256_ctx;

// Initialize SHA256 context
void sha256_init(sha256_ctx *ctx);

// Update SHA256 context with input data
void sha256_update(sha256_ctx *ctx, const uint8_t *data, size_t len);

// Finalize SHA256 computation and produce digest
void sha256_final(sha256_ctx *ctx, uint8_t *digest);

// Convenience function: hash input buffer into output digest (32 bytes)
void sha256(const uint8_t *data, size_t len, uint8_t *digest);

#endif /* SHA2_H */
