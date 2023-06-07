/* Samples k words from an incoming text file of unknown length n.
 * Makes use of a deque that stores at most k items. Chance
 * of a word being sampled is k/n. This is ensured by making use
 * of a reservoir sampling algorithm (see below).
 * Provided is 'shakespeare.txt', containing the complete works of
 * Shakespeare to sample from.
 */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "deque.h"

DEFINE_DEQUE_TYPE(char *, string);

int main(int argc, char *argv[])
{
    // Number of items to return from permutation
    int k;
    // Filename
    char *filename;

    // Check usage
    if (argc != 3)
    {
        printf("Usage: ./permutation k filename\n");
        return 1;
    }

    k = atoi(argv[1]);
    if (k < 1)
    {
        printf("Value of k has to be 1 or higher.");
        return 1;
    }

    filename = argv[2];

    // Load file
    FILE *infile_ptr = fopen(filename, "r");
    if (infile_ptr == NULL)
    {
        printf("Cannot to open '%s'\n", filename);
        return 1;
    }

    // Create pointer to deque
    string_deque *deque = deque_string_create();

    // Use nanosecond time of CPU clock to seed RNG
    struct timeval timer;
    gettimeofday(&timer, NULL);
    srand((timer.tv_sec * 1000) + (timer.tv_usec / 1000));
    int choice;



    // Word buffer, with max word length
    int max_length = 40;
    char word[max_length + 1];

    // Iteration
    int index = 0, words = 0;

    char c;
    while (fread(&c, sizeof(char), 1, infile_ptr))
    {
        if (isalpha(c) || ((c == '\'' || c == '-') && index > 0))
        {
            word[index] = c;
            index++;

            if (index > max_length)
            {
                // Consume rest of string
                while (fread(&c, sizeof(char), 1, infile_ptr) && isalpha(c));
                index = 0;
            }
        }
        else if (isdigit(c))
        {
            while (fread(&c, sizeof(char), 1, infile_ptr) && isalpha(c));
            index = 0;
        }

        else if (index > 0)
        {
            word[index] = '\0';
            //printf("%s\n", word);
            words++;

            if (deque->size == k)
            {
                // Algorithm R for reservoir sampling (independent of input size N)
                // See https://florian.github.io/reservoir-sampling/ or the wikipedia page
                // First, fill the data structure of size k with indices 0..k. Then go
                // through input indices k+1..n-1, with chance k/i to replace an element
                int replace = rand() % words;
                if (replace <= k)
                {
                    choice = rand() % 100 + 1;
                    char *removed_word;
                    if (choice <= 50)
                    {
                        removed_word = deque_string_remove_first(deque);
                        free(removed_word);
                        removed_word = NULL;
                    }
                    else
                    {
                        removed_word = deque_string_remove_last(deque);
                        free(removed_word);
                        removed_word = NULL;
                    }
                }
                else
                {
                    index = 0;
                    continue;
                }
            }

            char *selected_word = malloc(index + 1);
            strcpy(selected_word, word);


            choice = rand() % 4 + 1;
            if (choice == 1)
            {
                deque_string_add_first(deque, selected_word);
            }
            else if (choice == 2)
            {
                deque_string_add_last(deque, selected_word);
            }
            else if (choice == 3)
            {
                if (deque->size > 0)
                {
                    char *swap = deque_string_remove_first(deque);
                    deque_string_add_first(deque, selected_word);
                    deque_string_add_first(deque, swap);
                }
                else
                {
                    deque_string_add_first(deque, selected_word);
                }

            }
            else if (choice == 4)
            {
                if (deque->size > 0)
                {
                    char *swap = deque_string_remove_last(deque);
                    deque_string_add_last(deque, selected_word);
                    deque_string_add_last(deque, swap);
                }
                else
                {
                    deque_string_add_last(deque, selected_word);
                }

            }
            index = 0;
        }
    }

    int filename_length = strlen(filename);
    for (int i = 0; i < filename_length; i++)
    {
        printf("-");
    }
    printf("\n");
    for (int i = 0; i < filename_length; i++)
    {
        printf("%c", toupper(filename[i]));
    }
    printf("\n");
    for (int i = 0; i < filename_length; i++)
    {
        printf("-");
    }
    printf("\n");

    printf("Sampled %i out of a total of %i words:\n\n", k, words);
    for (int i = 0; i < k; i++)
    {
        printf("- %s\n", deque_string_iterator_next(deque));
    }
    printf("\n");

    char *removed;
    for (int i = 0; i < k; i++)
    {
        removed = deque_string_remove_last(deque);
        free(removed);
        removed = NULL;
    }


    deque_string_free(deque);
    fclose(infile_ptr);
    return 0;
}
