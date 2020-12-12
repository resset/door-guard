#ifndef _MQ_H_
#define _MQ_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    MQ_FALSE = 0,
    MQ_TRUE = 1,
    MQ_SUCCESS = 2,
    MQ_ERROR = 3
} mq_result_t;

typedef struct {
    char *buffer;
    size_t size;
    size_t first;
    size_t elements_n;
} mq_t;

mq_result_t mq_init(mq_t * mq, char * buffer, size_t size);
mq_result_t mq_push(mq_t * mq, const char * message);
mq_result_t mq_pop(mq_t * mq, char * message);
mq_result_t mq_is_empty(mq_t * mq);
mq_result_t mq_is_full(mq_t * mq);

#ifdef __cplusplus
}
#endif

#endif /* _MQ_H_ */
