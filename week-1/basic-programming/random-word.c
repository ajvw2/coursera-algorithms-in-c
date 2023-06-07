#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LENGTH 50

bool bernoulli(int i);

int main(int argc, char *argv[])
{
    // Ensure correct usage
    if (argc < 2)
    {
        printf("Usage: ./random-word word-1 word-2 ... word-n OR ./random-word filename.txt\n");
        return 1;
    }

    // Allocate room for bool stating if a file has to be opened or not
    bool use_filename;

    // Allocate room for file extension
    char extension[5];

    // Check if filename is being passed, or multiple words
    if (argc == 2)
    {
        // Save length of filename
        int length = strlen(argv[1]);
        if (length <= 4)
        {
            use_filename = false;
        }
        else
        {
            // Save last 4 characters of input filename to extension
            for (int i = length - 4; i <= length; i++)
            {
                extension[i + 4 - length] = argv[1][i];
            }
        }
    }

    if (strcmp(extension, ".txt") == 0)
    {
        use_filename = true;
    }

    // Declare winning word (max length 50)
    char winner[MAX_LENGTH];

    // Initialize random number generator
    srand(time(NULL));

    if (!use_filename)
    {
        // Pick one of the command-line arguments randomly using
        // Knuth's method (when reading the ith word, select it with
        // probability 1/i to be the champion, replacing the previous
        // champion).

        for (int i = 1; i < argc; i++)
        {
            if (bernoulli(i))
            {
                strcpy(winner, argv[i]);
            }
        }
    }
    else
    {
        // Open file
        FILE *infile_ptr = fopen(argv[1], "r");
        if (infile_ptr == NULL)
        {
            printf("Could not to open file.\n");
            return 1;
        }

        // Buffer for character
        char c;

        // Buffer for word
        char buffer[MAX_LENGTH];

        // Create iterator for word
        int i = 1;

        // Create iterator for random generator
        int n = 0;

        // Loop over words in file. Set to winner if bernoulli(i)
        while (fread(&c, sizeof(char), 1, infile_ptr))
        {
            if (c != '\n' && c != ' ')
            {
                buffer[n] = c;
                n++;
            }
            else
            {
                buffer[n] = '\0';
                n = 0;
                if (bernoulli(i))
                {
                    strcpy(winner, buffer);
                }
                i++;
            }
        }

        // Close file
        fclose(infile_ptr);
    }

    // Print winner
    printf("Chosen word: %s\n", winner);
    return 0;
}

bool bernoulli(int i)
{
    // Returns 'true' with probability p = 1/i and false with probability 1-p
    // Get p
    float p = 1 / (float)i;

    // Generate random number between 0 and 1
    float n = rand() / (float)RAND_MAX;

    // Return
    if (n <= p)
    {
        return true;
    }
    return false;
}