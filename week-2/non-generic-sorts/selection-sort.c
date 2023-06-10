// Selection sort algorithm for an array of ints
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void populate_array(int *array, int size);
void print_array(int *array, int size);
void selection_sort(int *array, int size);
bool compare(int a, int b);
void exchange(int *array, int index_a, int index_b);

int main(void)
{
    int size = 25;
    int array[size];

    // Populate array with random numbers between 0 and 99
    populate_array(array, size);
    print_array(array, size);

    // Use selection sort to sort the array
    selection_sort(array, size);
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

void selection_sort(int *array, int size)
{
    for (int i = 0; i < size; i++)
    {
        int minimum_index = i;
        for (int j = i; j < size; j++)
        {
            if (compare(array[minimum_index], array[j])) {
                minimum_index = j;
            }
        }
        exchange(array, i, minimum_index);
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