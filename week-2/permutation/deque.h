/* Generic DEQUE datastructure in C (using macro's)
 * Credit to https://codereview.stackexchange.com/questions/277643/a-sort-of-generic-stack-implementation-in-c-using-macros
 * for giving me a guideline for how to start implementing this in C*/

#ifndef DEQUE_H
#define DEQUE_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const size_t MINIMUM_CAPACITY = 8;

#define DEFINE_DEQUE_TYPE(T, prefix)                                                                                   \
    typedef struct prefix##_deque                                                                                      \
    {                                                                                                                  \
        int capacity;                                                                                                  \
        int size;                                                                                                      \
        int first;                                                                                                     \
        int last;                                                                                                      \
        T *storage_array;                                                                                              \
        bool reset_iterator;                                                                                           \
        int next_item;                                                                                                 \
        bool has_next;                                                                                                 \
    } prefix##_deque;                                                                                                  \
                                                                                                                       \
    /* Return pointer to deque */                                                                                      \
    prefix##_deque *deque_##prefix##_create(void)                                                                      \
    {                                                                                                                  \
        prefix##_deque *deque = malloc(sizeof(*deque));                                                                \
                                                                                                                       \
        assert(deque);                                                                                                 \
        deque->capacity = MINIMUM_CAPACITY;                                                                            \
        deque->size = 0;                                                                                               \
        deque->first = 0;                                                                                              \
        deque->last = 0;                                                                                               \
        assert((deque->storage_array = malloc(sizeof(*deque->storage_array) * deque->capacity)));                      \
        deque->reset_iterator = true;                                                                                  \
        deque->next_item = 0;                                                                                          \
        deque->has_next = false;                                                                                       \
        return deque;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    /* Free memory allocated for deque */                                                                              \
    void deque_##prefix##_free(prefix##_deque *deque)                                                                  \
    {                                                                                                                  \
        assert(deque);                                                                                                 \
        free(deque->storage_array);                                                                                    \
        deque->storage_array = NULL;                                                                                   \
        free(deque);                                                                                                   \
        deque = NULL;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    /* Resize deque */                                                                                                 \
    void deque_##prefix##_resize(prefix##_deque *deque, int increase)                                                  \
    {                                                                                                                  \
        if (!increase)                                                                                                 \
        {                                                                                                              \
            /* Halve array capacity */                                                                                 \
            deque->capacity /= 2;                                                                                      \
            T *tmp = malloc(deque->capacity * sizeof(deque->storage_array));                                           \
            assert(tmp);                                                                                               \
            for (int i = deque->first; i < deque->last; i++)                                                           \
            {                                                                                                          \
                if (i < 0)                                                                                             \
                {                                                                                                      \
                    tmp[i - deque->first] = deque->storage_array[(deque->capacity * 2) + (i % (deque->capacity * 2))]; \
                }                                                                                                      \
                else                                                                                                   \
                {                                                                                                      \
                    tmp[i - deque->first] = deque->storage_array[i % (deque->capacity * 2)];                           \
                }                                                                                                      \
            }                                                                                                          \
            deque->first = 0;                                                                                          \
            deque->last = deque->size;                                                                                 \
            free(deque->storage_array);                                                                                \
            deque->storage_array = tmp;                                                                                \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            /* Double array capacity */                                                                                \
            deque->capacity *= 2;                                                                                      \
            T *tmp = malloc(deque->capacity * sizeof(deque->storage_array));                                           \
            assert(tmp);                                                                                               \
            for (int i = deque->first; i < deque->last; i++)                                                           \
            {                                                                                                          \
                if (i < 0)                                                                                             \
                {                                                                                                      \
                    tmp[i - deque->first] = deque->storage_array[(deque->capacity / 2) + (i % (deque->capacity / 2))]; \
                }                                                                                                      \
                else                                                                                                   \
                {                                                                                                      \
                    tmp[i - deque->first] = deque->storage_array[i % (deque->capacity / 2)];                           \
                }                                                                                                      \
            }                                                                                                          \
            deque->first = 0;                                                                                          \
            deque->last = deque->size;                                                                                 \
            free(deque->storage_array);                                                                                \
            deque->storage_array = tmp;                                                                                \
        }                                                                                                              \
        return;                                                                                                        \
    }                                                                                                                  \
                                                                                                                       \
    /* Returns true if deque is empty */                                                                               \
    bool deque_##prefix##_is_empty(const prefix##_deque *deque)                                                        \
    {                                                                                                                  \
        assert(deque);                                                                                                 \
        return deque->size == 0;                                                                                       \
    }                                                                                                                  \
                                                                                                                       \
    /* Add item to the front of the deque */                                                                           \
    void deque_##prefix##_add_first(prefix##_deque *deque, T item)                                                     \
    {                                                                                                                  \
        assert(deque);                                                                                                 \
        if (deque->size == deque->capacity)                                                                            \
        {                                                                                                              \
            deque_##prefix##_resize(deque, 1);                                                                         \
        }                                                                                                              \
                                                                                                                       \
        if (deque->size == 0)                                                                                          \
        {                                                                                                              \
            deque->size++;                                                                                             \
            deque->first = 0;                                                                                          \
            deque->last = 1;                                                                                           \
            deque->storage_array[0] = item;                                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            deque->size++;                                                                                             \
            deque->first--;                                                                                            \
            int index;                                                                                                 \
            if (deque->first < 0)                                                                                      \
            {                                                                                                          \
                index = deque->capacity + (deque->first % deque->capacity);                                            \
                deque->storage_array[index] = item;                                                                    \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                index = deque->first % deque->capacity;                                                                \
                deque->storage_array[deque->first % deque->capacity] = item;                                           \
            }                                                                                                          \
        }                                                                                                              \
        deque->reset_iterator = true;                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    /* Add item to the back of the deque */                                                                            \
    void deque_##prefix##_add_last(prefix##_deque *deque, T item)                                                      \
    {                                                                                                                  \
        assert(deque);                                                                                                 \
        if (deque->size == deque->capacity)                                                                            \
        {                                                                                                              \
            deque_##prefix##_resize(deque, 1);                                                                         \
        }                                                                                                              \
        if (deque->size == 0)                                                                                          \
        {                                                                                                              \
            deque->size++;                                                                                             \
            deque->first = 0;                                                                                          \
            deque->last = 1;                                                                                           \
            deque->storage_array[0] = item;                                                                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            deque->size++;                                                                                             \
            int index;                                                                                                 \
            if (deque->last < 0)                                                                                       \
            {                                                                                                          \
                index = deque->capacity + (deque->last % deque->capacity);                                             \
                deque->storage_array[index] = item;                                                                    \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                index = deque->last % deque->capacity;                                                                 \
                deque->storage_array[index] = item;                                                                    \
            }                                                                                                          \
            deque->last++;                                                                                             \
            deque->reset_iterator = true;                                                                              \
        }                                                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    /* Remove item from front of deck */                                                                               \
    T deque_##prefix##_remove_first(prefix##_deque *deque)                                                             \
    {                                                                                                                  \
        assert(deque);                                                                                                 \
        assert(deque->size > 0);                                                                                       \
        if ((deque->size <= (deque->capacity / 4)) && (deque->capacity > MINIMUM_CAPACITY))                            \
        {                                                                                                              \
            deque_##prefix##_resize(deque, 0);                                                                         \
        }                                                                                                              \
                                                                                                                       \
        deque->size--;                                                                                                 \
        T item;                                                                                                        \
        if (deque->first < 0)                                                                                          \
        {                                                                                                              \
            item = deque->storage_array[deque->capacity + (deque->first % deque->capacity)];                           \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            item = deque->storage_array[deque->first % deque->capacity];                                               \
        }                                                                                                              \
        deque->first++;                                                                                                \
        deque->reset_iterator = true;                                                                                  \
        return item;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    /* Remove item from back of deque */                                                                               \
    T deque_##prefix##_remove_last(prefix##_deque *deque)                                                              \
    {                                                                                                                  \
        assert(deque);                                                                                                 \
        assert(deque->size > 0);                                                                                       \
        if ((deque->size <= (deque->capacity / 4)) && (deque->capacity > MINIMUM_CAPACITY))                            \
        {                                                                                                              \
            deque_##prefix##_resize(deque, 0);                                                                         \
        }                                                                                                              \
                                                                                                                       \
        deque->size--;                                                                                                 \
        deque->last--;                                                                                                 \
        T item;                                                                                                        \
        if (deque->last < 0)                                                                                           \
        {                                                                                                              \
            item = deque->storage_array[deque->capacity + (deque->last % deque->capacity)];                            \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            item = deque->storage_array[deque->last % deque->capacity];                                                \
        }                                                                                                              \
        deque->reset_iterator = true;                                                                                  \
        return item;                                                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    /* Iterator */                                                                                                     \
    T deque_##prefix##_iterator_next(prefix##_deque *deque)                                                            \
    {                                                                                                                  \
        assert(deque);                                                                                                 \
        assert(deque->size > 0);                                                                                       \
        if (deque->reset_iterator)                                                                                     \
        {                                                                                                              \
            deque->next_item = deque->first;                                                                           \
            if (deque->size == 1)                                                                                      \
            {                                                                                                          \
                deque->has_next = false;                                                                               \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                deque->has_next = true;                                                                                \
            }                                                                                                          \
            deque->reset_iterator = false;                                                                             \
        }                                                                                                              \
                                                                                                                       \
        T item;                                                                                                        \
        if (deque->next_item < 0)                                                                                      \
        {                                                                                                              \
            item = deque->storage_array[deque->capacity + (deque->next_item % deque->capacity)];                       \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            item = deque->storage_array[deque->next_item % deque->capacity];                                           \
        }                                                                                                              \
                                                                                                                       \
        deque->next_item++;                                                                                            \
        if (deque->next_item >= deque->last)                                                                           \
        {                                                                                                              \
            deque->has_next = false;                                                                                   \
        }                                                                                                              \
        return item;                                                                                                   \
    }
#endif