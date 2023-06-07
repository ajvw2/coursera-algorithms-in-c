/* Generic RANDOMIZED QUEUE datastructure
 * Credit to https://codereview.stackexchange.com/questions/277643/a-sort-of-generic-stack-implementation-in-c-using-macros
 */

#ifndef RQUEUE_H
#define RQUEUE_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

static const int MINIMUM_CAPACITY = 8;

#define DEFINE_RQUEUE_TYPE(T, prefix)                                                                                                \
    typedef struct prefix##_rqueue                                                                                                   \
    {                                                                                                                                \
        int capacity;                                                                                                                \
        int size;                                                                                                                    \
        T *storage_array;                                                                                                            \
        bool reset_iterator;                                                                                                         \
        int next_item;                                                                                                               \
        bool has_next;                                                                                                               \
    } prefix##_rqueue;                                                                                                               \
                                                                                                                                     \
    prefix##_rqueue *rqueue_##prefix##_create(void)                                                                                  \
    {                                                                                                                                \
        prefix##_rqueue *randomized_queue = malloc(sizeof(*randomized_queue));                                                       \
        assert(randomized_queue);                                                                                                    \
        randomized_queue->capacity = MINIMUM_CAPACITY;                                                                               \
        randomized_queue->size = 0;                                                                                                  \
        randomized_queue->storage_array = malloc(randomized_queue->capacity * sizeof(*randomized_queue->storage_array));             \
        assert(randomized_queue->storage_array);                                                                                     \
        randomized_queue->reset_iterator = true;                                                                                     \
        randomized_queue->next_item = 0;                                                                                             \
        randomized_queue->has_next = false;                                                                                          \
                                                                                                                                     \
        /* Seed RNG using nanosecond CPU time */                                                                                     \
        struct timeval timer;                                                                                                        \
        gettimeofday(&timer, NULL);                                                                                                  \
        srand((timer.tv_sec * 1000) + (timer.tv_usec / 1000));                                                                       \
                                                                                                                                     \
        return randomized_queue;                                                                                                     \
    }                                                                                                                                \
                                                                                                                                     \
    void rqueue_##prefix##_free(prefix##_rqueue *randomized_queue)                                                                   \
    {                                                                                                                                \
        assert(randomized_queue);                                                                                                    \
        free(randomized_queue->storage_array);                                                                                       \
        randomized_queue->storage_array = NULL;                                                                                      \
        free(randomized_queue);                                                                                                      \
        randomized_queue = NULL;                                                                                                     \
    }                                                                                                                                \
                                                                                                                                     \
    bool rqueue_##prefix##_is_empty(prefix##_rqueue *randomized_queue)                                                               \
    {                                                                                                                                \
        assert(randomized_queue);                                                                                                    \
        return randomized_queue->size == 0;                                                                                          \
    }                                                                                                                                \
                                                                                                                                     \
    void rqueue_##prefix##_enqueue(prefix##_rqueue *randomized_queue, T item)                                                        \
    {                                                                                                                                \
        assert(randomized_queue);                                                                                                    \
        if (randomized_queue->size == randomized_queue->capacity)                                                                    \
        {                                                                                                                            \
            randomized_queue->capacity *= 2;                                                                                         \
            T *tmp = realloc(randomized_queue->storage_array, randomized_queue->capacity * sizeof(randomized_queue->storage_array)); \
            assert(tmp);                                                                                                             \
            randomized_queue->storage_array = tmp;                                                                                   \
        }                                                                                                                            \
        randomized_queue->storage_array[randomized_queue->size] = item;                                                              \
        randomized_queue->size++;                                                                                                    \
        randomized_queue->reset_iterator = true;                                                                                     \
    }                                                                                                                                \
                                                                                                                                     \
    T rqueue_##prefix##_dequeue(prefix##_rqueue *randomized_queue)                                                                   \
    {                                                                                                                                \
        assert(randomized_queue);                                                                                                    \
        assert(randomized_queue->size > 0);                                                                                          \
        if (randomized_queue->size <= (randomized_queue->capacity / 4) && randomized_queue->size > MINIMUM_CAPACITY)                 \
        {                                                                                                                            \
            randomized_queue->capacity /= 2;                                                                                         \
            T *tmp = realloc(randomized_queue->storage_array, randomized_queue->capacity * sizeof(randomized_queue->storage_array)); \
            assert(tmp);                                                                                                             \
            randomized_queue->storage_array = tmp;                                                                                   \
        }                                                                                                                            \
        int chosen_index = rand() % randomized_queue->size;                                                                          \
        T item = randomized_queue->storage_array[chosen_index];                                                                      \
        randomized_queue->size--;                                                                                                    \
        randomized_queue->storage_array[chosen_index] = randomized_queue->storage_array[randomized_queue->size];                     \
        return item;                                                                                                                 \
    }                                                                                                                                \
                                                                                                                                     \
    T rqueue_##prefix##_iterator_next(prefix##_rqueue *randomized_queue)                                                             \
    {                                                                                                                                \
        assert(randomized_queue);                                                                                                    \
        assert(randomized_queue->size > 0);                                                                                          \
        if (randomized_queue->reset_iterator)                                                                                        \
        {                                                                                                                            \
            randomized_queue->next_item = 0;                                                                                         \
            if (randomized_queue->size == 1)                                                                                         \
            {                                                                                                                        \
                randomized_queue->has_next = false;                                                                                  \
            }                                                                                                                        \
            else                                                                                                                     \
            {                                                                                                                        \
                randomized_queue->has_next = true;                                                                                   \
            }                                                                                                                        \
            randomized_queue->reset_iterator = false;                                                                                \
        }                                                                                                                            \
                                                                                                                                     \
        T item = randomized_queue->storage_array[randomized_queue->next_item];                                                       \
        randomized_queue->next_item++;                                                                                               \
        if (randomized_queue->next_item >= randomized_queue->size - 1)                                                               \
        {                                                                                                                            \
            randomized_queue->has_next = false;                                                                                      \
        }                                                                                                                            \
        return item;                                                                                                                 \
    }

#endif