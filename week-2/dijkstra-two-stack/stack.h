/* Generic STACK datastructure
 * Credit to https://codereview.stackexchange.com/questions/277643/a-sort-of-generic-stack-implementation-in-c-using-macros
 */

#ifndef STACK_H
#define STACK_H
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const int MINIMUM_CAPACITY = 8;

#define DEFINE_STACK_TYPE(T, prefix)                                                                \
    typedef struct prefix##_stack                                                                   \
    {                                                                                               \
        int capacity;                                                                               \
        int size;                                                                                   \
        T *storage_array;                                                                           \
        bool reset_iterator;                                                                        \
        int next_item;                                                                              \
        bool has_next;                                                                              \
    } prefix##_stack;                                                                               \
                                                                                                    \
    prefix##_stack *stack_##prefix##_create(void)                                                   \
    {                                                                                               \
        prefix##_stack *stack = malloc(sizeof(*stack));                                             \
                                                                                                    \
        assert(stack);                                                                              \
        stack->capacity = MINIMUM_CAPACITY;                                                         \
        stack->size = 0;                                                                            \
        assert((stack->storage_array = malloc(sizeof(*stack->storage_array) * stack->capacity)));   \
        stack->reset_iterator = true;                                                               \
        stack->next_item = 0;                                                                       \
        stack->has_next = false;                                                                    \
        return stack;                                                                               \
    }                                                                                               \
                                                                                                    \
    void stack_##prefix##_free(prefix##_stack *stack)                                               \
    {                                                                                               \
        assert(stack);                                                                              \
        free(stack->storage_array);                                                                 \
        stack->storage_array = NULL;                                                                \
        free(stack);                                                                                \
        stack = NULL;                                                                               \
    }                                                                                               \
                                                                                                    \
    bool stack_##prefix##_is_empty(prefix##_stack *stack)                                           \
    {                                                                                               \
        assert(stack);                                                                              \
        return stack->size == 0;                                                                    \
    }                                                                                               \
                                                                                                    \
    void stack_##prefix##_push(prefix##_stack *stack, T item)                                       \
    {                                                                                               \
        assert(stack);                                                                              \
        if (stack->size == stack->capacity)                                                         \
        {                                                                                           \
            stack->capacity *= 2;                                                                   \
            T *tmp = realloc(stack->storage_array, sizeof(stack->storage_array) * stack->capacity); \
            assert(tmp);                                                                            \
            stack->storage_array = tmp;                                                             \
        }                                                                                           \
        stack->storage_array[stack->size] = item;                                                   \
        stack->size++;                                                                              \
        stack->reset_iterator = true;                                                               \
    }                                                                                               \
                                                                                                    \
    T stack_##prefix##_pop(prefix##_stack *stack)                                                   \
    {                                                                                               \
        assert(stack);                                                                              \
        assert(stack->size > 0);                                                                    \
        if (stack->size <= (stack->capacity / 4) && stack->size > MINIMUM_CAPACITY)                 \
        {                                                                                           \
            stack->capacity /= 2;                                                                   \
            T *tmp = realloc(stack->storage_array, sizeof(stack->storage_array) * stack->capacity); \
            assert(tmp);                                                                            \
            stack->storage_array = tmp;                                                             \
        }                                                                                           \
        stack->size--;                                                                              \
        T item = stack->storage_array[stack->size];                                                 \
        stack->reset_iterator = true;                                                               \
        return item;                                                                                \
    }                                                                                               \
                                                                                                    \
    T stack_##prefix##_iterator_next(prefix##_stack *stack)                                         \
    {                                                                                               \
        assert(stack);                                                                              \
        assert(stack->size > 0);                                                                    \
        if (stack->reset_iterator)                                                                  \
        {                                                                                           \
            stack->next_item = stack->size - 1;                                                     \
            if (stack->size == 1)                                                                   \
            {                                                                                       \
                stack->has_next = false;                                                            \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                stack->has_next = true;                                                             \
            }                                                                                       \
            stack->reset_iterator = false;                                                          \
        }                                                                                           \
                                                                                                    \
        T item = stack->storage_array[stack->next_item];                                            \
        stack->next_item--;                                                                         \
        if (stack->next_item <= 0)                                                                  \
        {                                                                                           \
            stack->has_next = false;                                                                \
        }                                                                                           \
        return item;                                                                                \
    }

#endif