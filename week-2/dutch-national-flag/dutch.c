// Sorts an array consisiting of N buckets, which each contain
// either "red", "white", or "blue".
// Can only use operations color(i), which checks the color inside
// a bucket, and swap(i, j), which swaps the order of two buckets.
// Requirement: ~N time.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fill_buckets(char *buckets, int size);
void print_bucket_contents(char *buckets, int size);
void sort_buckets(char *buckets, int size);
void swap(char *buckets, int i, int j);

int main(void){
    int size = 250;
    char buckets[size];

    // Initialize array
    fill_buckets(buckets, size);
    print_bucket_contents(buckets, size);

    // Sort the array
    sort_buckets(buckets, size);
    print_bucket_contents(buckets, size);
}

void fill_buckets(char *buckets, int size)
{
    srand(time(NULL));

    for (int i = 0; i < size; i++)
    {
        int color = rand() % 3;

        switch (color) {
            case 0:
                buckets[i] = 'r';
                break;
            case 1:
                buckets[i] = 'w';
                break;
            case 2:
                buckets[i] = 'b';
                break;
        }
    }
}

void print_bucket_contents(char *buckets, int size)
{
    for (int i = 0; i < size; i++)
    {
        if (buckets[i] == 'r')
        {
            printf("\033[1;31m"); 
        }
        else if (buckets[i] == 'w')
        {
            printf("\033[1;37m");
        }
        else if (buckets[i] == 'b')
        {
            printf("\033[1;34m");
        }
        printf("%c", buckets[i]);
        printf("\033[0m");
        
    }
    printf("\n");
}

void sort_buckets(char *buckets, int size)
{
    int white_start = 0;
    int blue_start = 0;

    for (int i = 0; i < size; i++)
    {
        // Two swaps necessary when bucket contains red, one when it contains white and zero when it contains blue.
        // With perfectly random bucket contents this results in ~N time on average.  
        if (buckets[i] == 'r')
        {
            swap(buckets, i, white_start++);
            if (buckets[i] != 'b')
            {
                swap(buckets, i, blue_start);
            }   
            blue_start++;
        }
        else if (buckets[i] == 'w')
        {
            swap(buckets, i, blue_start++);
        }
    }
}

void swap(char *buckets, int i, int j)
{
    char swap = buckets[i];
    buckets[i] = buckets[j];
    buckets[j] = swap;
}