#include <stdio.h>

int main(int argc, char *argv[])
{
    // Check correct usage.
    if (argc != 3)
    {
        printf("Usage: ./hello-goodbye name1 name2\n");
        return 1;
    }

    // Save names to variables
    char *name1 = argv[1];
    char *name2 = argv[2];

    // Print sentences
    printf("Hello %s and %s.\n", name1, name2);
    printf("Goodbye %s and %s.\n", name2, name1);

    return 0;
}