#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "mq.h"

int main(void)
{
    char buffer[20];
    char * message;
    message = malloc(sizeof(char) * 20);

    mq_t queue;
    mq_init(&queue, buffer, (size_t)20);
    if (MQ_ERROR == mq_push(&queue, "abcdefghijklmn")) {
        printf("1: error\n");
    }
    if (MQ_ERROR == mq_push(&queue, "123")) {
        printf("2: error\n");
    }
    if (MQ_ERROR == mq_push(&queue, "zyx")) {
        printf("3: error\n");
    }
    if (MQ_SUCCESS == mq_pop(&queue, message)) {
        printf("read message: %s\n", message);
    }
    if (MQ_ERROR == mq_push(&queue, "zyx")) {
        printf("3: error\n");
    }
    if (MQ_SUCCESS == mq_pop(&queue, message)) {
        printf("read message: %s\n", message);
    }
    if (MQ_SUCCESS == mq_pop(&queue, message)) {
        printf("read message: %s\n", message);
    }
    if (MQ_SUCCESS == mq_pop(&queue, message)) {
        printf("read message: %s\n", message);
    }

    free(message);

    return 0;
}
