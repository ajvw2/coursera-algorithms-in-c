// Insertion sort algorithm for an array of ints
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void populate_array(int *array, int size);
void print_array(int *array, int size);
void insertion_sort(int *array, int size);
bool compare(int a, int b);
void exchange(int *array, int index_a, int index_b);

int main(void)
{
    int size = 25;
    int array[size];

    // Populate array with random numbers between 0 and 99
    populate_array(array, size);
    print_array(array, size);

    // Use insertion sort to sort the array
    insertion_sort(array, size);
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

void insertion_sort(int *array, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = i; j > 0; j--)
        {
            if (compare(array[j - 1], array[j])) {
                exchange(array, j, j - 1);
            }
        }
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