#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "mq.h"

mq_result_t mq_init(mq_t * mq, char * buffer, size_t size)
{
    if (NULL != mq && NULL != buffer && 0 < size) {
        mq->buffer = buffer;
        mq->size = size;
        mq->first = 0;
        mq->elements_n = 0;

        return MQ_SUCCESS;
    } else {
        return MQ_ERROR;
    }
}

void enqueue(mq_t * mq, char byte)
{
    mq->buffer[(mq->first + mq->elements_n) % mq->size] = byte;
    ++mq->elements_n;
}

mq_result_t mq_push(mq_t * mq, const char * message)
{
    if (NULL != mq && NULL != message && mq->elements_n + strlen(message) + 1 <= mq->size) {
        for (; *message; message++) {
            enqueue(mq, *message);
        }
        enqueue(mq, '\0');
        return MQ_SUCCESS;
    } else {
        return MQ_ERROR;
    }
}

void dequeue(mq_t * mq, char * byte)
{
    *byte = mq->buffer[mq->first];
    mq->first = (mq->first + 1) % mq->size;
    --mq->elements_n;
}

mq_result_t mq_pop(mq_t * mq, char * message)
{
    if (NULL != mq && 0 != mq->elements_n) {
        while (mq->buffer[mq->first] != '\0') {
            dequeue(mq, message);
            message++;
        }
        dequeue(mq, message);

        return MQ_SUCCESS;
    } else {
        return MQ_ERROR;
    }
}

mq_result_t mq_is_empty(mq_t * mq)
{
    if (NULL != mq) {
        if (0 == mq->elements_n) {
            return MQ_TRUE;
        } else {
            return MQ_FALSE;
        }
    } else {
        return MQ_ERROR;
    }
}

mq_result_t mq_is_full(mq_t * mq)
{
    if (NULL != mq) {
        if (mq->size == mq->elements_n) {
            return MQ_TRUE;
        } else {
            return MQ_FALSE;
        }
    } else {
        return MQ_ERROR;
    }
}
