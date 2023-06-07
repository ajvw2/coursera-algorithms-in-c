#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "rqueue.h"

DEFINE_RQUEUE_TYPE(char *, string);

int main(void)
{
    // Get randomized queue
    string_rqueue *rqueue = rqueue_string_create();

    char *doremi[] = {"ut", "queant", "laxis", "resonare", "fibris", "mira", "gestorum", "famuli", "tuorum", "solve", "polluti", "labii", "reatum", "sancte", "iohannes"};

    for (int i = 0; i < 15; i++)
    {
        rqueue_string_enqueue(rqueue, doremi[i]);
    }

    printf("Randomized queue contents:\n");
    for (int i = 0; i < rqueue->size; i++)
    {
        printf("- %s\n", rqueue_string_iterator_next(rqueue));
    }

    printf("\nDequeued random items:\n");
    for (int i = 0; i < 15; i++)
    {
        printf("- %s\n", rqueue_string_dequeue(rqueue));
    }

    printf("\n");
}