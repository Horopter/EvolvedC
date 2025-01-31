#ifndef EC_STATUS_H
#define EC_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ec_status_t - Standard error/status codes for EC
 */
typedef enum {
    EC_OK = 0,             /* Operation succeeded */
    EC_ERR_NOT_FOUND,      /* Requested entry not found */
    EC_ERR_INVALID_OP,     /* Invalid Operation */
    EC_ERR_ALLOC,          /* Memory allocation failed */
    EC_ERR_INVALID_ARG,    /* Invalid argument provided */
    EC_ERR_OUT_OF_BOUNDS,  /* Index out of bounds */
    EC_ERR_MAX_CAPACITY,   /* Exceeded maximum capacity */
    EC_ITER_END,           /* Iterator reached end */
    /* Add more codes as needed */
} ec_status_t;

#ifdef __cplusplus
}
#endif

#endif /* EC_STATUS_H */
