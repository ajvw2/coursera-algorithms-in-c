// Shell sorts a randomly generated array of ints, then
// shuffles this array randomly using Knuth's shuffle algorithm
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void populate_array(int *array, int size);
void print_array(int *array, int size);
void shell_sort(int *array, int size);
void shuffle(int *array, int size);
bool compare(int a, int b);
void exchange(int *array, int index_a, int index_b);

int main(void)
{
    int size = 40;
    int array[size];

    // Populate array with random numbers between 0 and 99
    populate_array(array, size);
    print_array(array, size);

    // Use shell sort to sort the array
    shell_sort(array, size);
    print_array(array, size);

    // Shuffle the array
    shuffle(array, size);
    print_array(array, size);

    return 0;
}

void populate_array(int *array, int size)
{
    srand(time(NULL));

    for (int i = 0; i < size; i++)
    {
        array[i] = rand() % 100;
    }
}

void print_array(int *array, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%i ", array[i]);
    }
    printf("\n");
}

void shell_sort(int *array, int size)
{
    // Knuth gap sequence, where h = 3*h + 1, not greater than N/3
    int h = 1;
    int next_h = 3 * h + 1;
    
    while (next_h <= size / 3)
    {
        h = next_h;
        next_h = 3 * h + 1; // 1, 4, 13, 40, 121, 364, ...
    }

    while (h >= 1)
    {
        // Insertion sort
        for (int i = h; i < size; i++)
        {
            for (int j = i; j >= h; j -= h)
            {
                if (compare(array[j - h], array[j])) {
                    exchange(array, j, j - h);
                }
            }
        }

        // Update h. Integer division gives correct result through
        // truncation.
        h = h / 3;
    }
    
}

void shuffle(int *array, int size)
{
    // Iterate through array, generate random index between 0 and i,
    // swap i with random index.
    for (int i = 0; i < size; i++)
    {
        int swap_index = rand() % (i + 1);
        exchange(array, swap_index, i);
    }
}


bool compare(int a, int b) {
    if (b < a) 
    {
        return true;
    }
    return false;
}

void exchange(int *array, int index_a, int index_b)
{
    int swap = array[index_a];
    array[index_a] = array[index_b];
    array[index_b] = swap;
}