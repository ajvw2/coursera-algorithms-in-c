/* Percolation model. See specifications at: 
 * https://coursera.cs.princeton.edu/algs4/assignments/percolation/specification.php 
 * 
 * User inputs a grid size and the number of trials to run into the command line.
 * During a trial, grid sites open randomly until the system percolates. The 
 * percolation threshold mean, standard deviation, and confidence intervals of
 * all trials are displayed.
 * Optionally, the user can add -v in the command line to enable the visualiser, which
 * displays the trial step by step in the command prompt (using ncurses). Grid sizes
 * and trial numbers are more limited. This feature is mostly educational, giving
 * insight into the way the algorithm works.
 */

#include <ncurses.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
// #include <windows.h>

typedef struct node
{
    int parent;
    int weight;
} node;

int percolation(int n);
void free_grid(void);
void get_display_grid(void);
void print_grid(WINDOW *trial_window);
int get_index(int row, int col);
bool open_site(int row, int col);
bool is_open(int row, int col);
node root(node p);
void quick_union(int p, int q);
bool union_find(int p, int q);
bool is_full(int row, int col);
int number_of_open_sites(void);
bool percolates(void);
int percolation_stats(int n, int trials);
double mean(double *thresholds, int trials);
double stddev(double mean, double *thresholds, int trials);
double confidence_lo(double mean, double sd, int trials);
double confidence_hi(double mean, double sd, int trials);
void clear_screen(void);

node *grid = NULL;
/* Status is a byte with following states
 * 000 = 0 = closed
 * 100 = 4 = open
 * 101 = 5 = open and connected to bottom
 * 110 = 6 = open and connected to top
 * 111 = 7 = open and connected to bottom and top
 * Can be updated with bitwise OR
 */
uint8_t *status_grid = NULL;
/* Display grid has following states:
 * 'c' = closed
 * 'o' = open
 * 'f' = full
 */
char *display_grid = NULL;

int N = 0;
int grid_size = 0; // = (N * N)
bool has_percolated;
bool visualize = false;

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "v")) != -1)
    {
        switch (opt)
        {
        case 'v':
            visualize = true;
            break;
        default:
            printf("Usage: ./percolation [-v] size trials\n");
            return 1;
        }
    }

    if (argc - optind < 2)
    {
        printf("Usage: ./percolation [-v] size trials\n");
        return 1;
    }

    int size_of_grid = atoi(argv[optind]);
    int number_of_trials = atoi(argv[optind + 1]);

    if (visualize)
    {
        // NCURSES START
        initscr();
        noecho();
        cbreak();
        curs_set(0);

        if (!has_colors())
        {
            printw("Terminal doesn't support color");
            getch();
            return 1;
        }
        start_color();

        init_pair(1, COLOR_BLACK, COLOR_BLACK);
        init_pair(2, COLOR_WHITE, COLOR_WHITE);
        init_pair(3, COLOR_BLUE, COLOR_BLUE);
        init_pair(4, COLOR_GREEN, COLOR_BLACK);
    }

    int return_value = percolation_stats(size_of_grid, number_of_trials);
    if (return_value != 0)
    {
        return 1;
    }

    if (visualize)
    {
        char exit_char;
        while (exit_char != 'x')
        {
            exit_char = getch();
        }
        endwin();
        // NCURSES END
    }

    return 0;
}

int percolation(int n)
{
    if (n <= 0)
    {
        printf("Error: Grid size equal to or smaller than 0.\n");
        return 1;
    }

    has_percolated = false;
    N = n;
    grid_size = N * N;

    grid = malloc(grid_size * sizeof(node));
    if (grid == NULL)
    {
        printf("Error: Failed to allocate memory for grid.\n");
        return 1;
    }

    status_grid = malloc(grid_size * sizeof(uint8_t));
    if (status_grid == NULL)
    {
        printf("Error: Failed to allocate memory for status grid.\n");
        return 1;
    }

    display_grid = malloc(grid_size * sizeof(char));
    if (display_grid == NULL)
    {
        printf("Error: Failed to allocate memory for display grid.\n");
        return 1;
    }

    // Initialize values for grids
    for (int i = 0; i < grid_size; i++)
    {
        grid[i].parent = i;
        grid[i].weight = 1;
        status_grid[i] = 0;
        display_grid[i] = 'c';
    }

    return 0;
}

void free_grid(void)
{
    free(grid);
    grid = NULL;
    free(status_grid);
    status_grid = NULL;
    free(display_grid);
    display_grid = NULL;

    // Reinitialize global variables
    N = 0;
    grid_size = 0;
    has_percolated = false;
}

void get_display_grid(void)
{
    for (int i = 0; i < grid_size; i++)
    {
        // Convert index to column and row number
        int col = (i % N) + 1;
        int row = (i - col + 1) / N + 1;

        if (is_full(row, col))
        {
            display_grid[i] = 'f';
        }
        else if (!is_full(row, col) && is_open(row, col))
        {
            display_grid[i] = 'o';
        }
        else
        {
            display_grid[i] = 'c';
        }
    }
}

void print_grid(WINDOW *trial_window)
{
    int y = 1;
    int x = 2;
    // Display character
    char c = ' ';

    for (int i = 0; i < grid_size; i++)
    {
        if ((i % N == 0))
        {
            y++;
            x = 2;
            wmove(trial_window, y, x);
        }

        if (display_grid[i] == 'f')
        {
            wattron(trial_window, COLOR_PAIR(3));
            wprintw(trial_window, "%c%c%c", c, c, c);
            wattroff(trial_window, COLOR_PAIR(3));
        }
        else if (display_grid[i] == 'o')
        {
            wattron(trial_window, COLOR_PAIR(2));
            wprintw(trial_window, "%c%c%c", c, c, c);
            wattroff(trial_window, COLOR_PAIR(2));
        }
        else if (display_grid[i] == 'c')
        {
            wattron(trial_window, COLOR_PAIR(1));
            wprintw(trial_window, "%c%c%c", c, c, c);
            wattroff(trial_window, COLOR_PAIR(1));
        }
    }
}

int get_index(int row, int col)
{
    // By convention, the row and column indices are integers
    // between 1 and n, where (1, 1) is the upper-left site.

    // Out of range
    if (row > N || row < 1 || col > N || col < 1)
    {
        return -1;
    }
    else
    {
        return (col - 1) + (row - 1) * N;
    }
}

bool open_site(int row, int col)
{
    int index = get_index(row, col);

    if (is_open(row, col))
    {
        return false;
    }

    if (row == 1)
    {
        status_grid[index] = 6;
    }
    else if (row == N)
    {
        status_grid[index] = 5;
    }
    else
    {
        status_grid[index] = 4;
    }

    // Union with newly opened site's 4 neighbours
    int left_neighbor = get_index(row, col - 1);
    if (left_neighbor != -1 && status_grid[left_neighbor])
    {
        quick_union(index, left_neighbor);
    }

    int right_neighbor = get_index(row, col + 1);
    if (right_neighbor != -1 && status_grid[right_neighbor])
    {
        quick_union(index, right_neighbor);
    }

    int top_neighbor = get_index(row - 1, col);
    if (top_neighbor != -1 && status_grid[top_neighbor])
    {
        quick_union(index, top_neighbor);
    }

    int bottom_neighbor = get_index(row + 1, col);
    if (bottom_neighbor != -1 && status_grid[bottom_neighbor])
    {
        quick_union(index, bottom_neighbor);
    }

    // Update status of root of newly opened site
    status_grid[root(grid[index]).parent] |= status_grid[index];

    if (status_grid[index] == 7)
    {
        has_percolated = true;
    }
    return true;
}

bool is_open(int row, int col)
{
    int index = get_index(row, col);
    return (bool)status_grid[index];
}

node root(node p)
{
    // Root is found when a node's parent is equal to its own index
    while (p.parent != grid[p.parent].parent)
    {
        // Path compression
        grid[p.parent].parent = grid[grid[p.parent].parent].parent;

        // Go to parent node
        p.parent = grid[p.parent].parent;
    }

    return p;
}

void quick_union(int p, int q)
{
    node root_p = root(grid[p]);
    node root_q = root(grid[q]);

    if (root_p.parent == root_q.parent)
    {
        return;
    }

    // Weighted quick union
    if (root_q.weight > root_p.weight)
    {
        grid[root_p.parent].parent = grid[root_q.parent].parent;
        grid[root_q.parent].weight += grid[root_p.parent].weight;
    }
    else
    {
        grid[root_q.parent].parent = grid[root_p.parent].parent;
        grid[root_p.parent].weight += grid[root_q.parent].weight;
    }

    // Newly opened index is always passed in first. Use bitwise OR to
    // update its percolation state with the state of the neighbor's root
    // (this prevents an extra lookup for the root of index_2)
    status_grid[p] |= status_grid[root_q.parent];
}

bool union_find(int p, int q)
{
    node root_p = root(grid[p]);
    node root_q = root(grid[q]);
    return root_p.parent == root_q.parent;
}

bool is_full(int row, int col)
{
    int index = get_index(row, col);

    // Full sites are connected to the top, so they
    // must have a byte value of at least 110 (= 6)
    if (status_grid[root(grid[index]).parent] >= 6)
    {
        return true;
    }
    return false;
}

int number_of_open_sites(void)
{
    int open_sites = 0;

    for (int i = 0; i < grid_size; i++)
    {
        if (status_grid[i])
        {
            open_sites++;
        }
    }

    return open_sites;
}

bool percolates(void)
{
    // Redundant function, included as per the assignment requirements
    return has_percolated;
}

int percolation_stats(int n, int trials)
{
    if (trials <= 0)
    {
        printf("Error: Number of trials equal to or smaller than 0.\n");
        return 1;
    }

    double percolation_thresholds[trials];

    WINDOW *trial_window;
    WINDOW *stat_window;
    WINDOW *info_window;
    char input_character;
    int trial_window_width;

    if (visualize)
    {
        if (n < 5)
        {
            trial_window_width = 18;
        }
        else
        {
            trial_window_width = (n * 3) + 4;
        }

        trial_window = newwin(n + 6, trial_window_width, 0, 0);
        stat_window = newwin(5, 60, n + 6, 0);
        info_window = newwin(4, 60, n + 11, 0);
    }

    double percolation_threshold_mean;
    double standard_deviation;
    double confidence_interval_low;
    double confidence_interval_high;

    // Use nanosecond time of CPU clock to seed RNG
    struct timeval timer;
    gettimeofday(&timer, NULL);
    srand((timer.tv_sec * 1000) + (timer.tv_usec / 1000));

    for (int trial = 0; trial < trials; trial++)
    {
        int return_value = percolation(n);

        if (return_value != 0)
        {
            return 1;
        }

        if (visualize)
        {
            mvwprintw(trial_window, 1, 1, "Trial %d/%d", trial + 1, trials);
            wrefresh(trial_window);
            wrefresh(stat_window);
            refresh();
        }

        // Randomly open up sites on grid until grid percolates
        while (!percolates())
        {
            bool has_opened = false;

            while (!has_opened)
            {
                int row = rand() % N + 1;
                int col = rand() % N + 1;

                has_opened = open_site(row, col);
            }

            if (visualize)
            {
                get_display_grid();

                print_grid(trial_window);
                mvwprintw(trial_window, n + 3, 1, "Open sites: %d/%d            ", number_of_open_sites(), grid_size);
                mvwprintw(trial_window, n + 4, 1, "            ");
                box(trial_window, 0, 0);
                wrefresh(trial_window);

                mvwprintw(info_window, 1, 1, "Hold 's' to continue current trial");
                mvwprintw(info_window, 2, 1, "Press 'x' to quit");
                box(info_window, 0, 0);
                wrefresh(info_window);

                refresh();

                while (true)
                {
                    input_character = getch();
                    if (input_character == 's')
                    {
                        break;
                    }
                    else if (input_character == 'x')
                    {
                        printf("Execution of program terminated by user.\n");
                        return 1;
                    }
                }
            }
        }

        double threshold = (double)number_of_open_sites() / grid_size;
        percolation_thresholds[trial] = threshold;

        free_grid();

        if (visualize)
        {
            percolation_threshold_mean = mean(percolation_thresholds, trial + 1);
            standard_deviation = stddev(percolation_threshold_mean, percolation_thresholds, trial + 1);
            confidence_interval_low = confidence_lo(percolation_threshold_mean, standard_deviation, trial + 1);
            confidence_interval_high = confidence_lo(percolation_threshold_mean, standard_deviation, trial + 1);

            wattron(trial_window, COLOR_PAIR(4));
            mvwprintw(trial_window, n + 4, 1, "Percolated!");
            wattroff(trial_window, COLOR_PAIR(4));
            mvwprintw(stat_window, 1, 1, "mean%*c = %.010f\n", 19, ' ', percolation_threshold_mean);
            mvwprintw(stat_window, 2, 1, "stddev%*c = %.010f\n", 17, ' ', standard_deviation);
            mvwprintw(stat_window, 3, 1, "95%% confidence interval = [%.010f, %.010f]\n", confidence_interval_low, confidence_interval_high);
            if (trial < trials - 1)
            {
                mvwprintw(info_window, 1, 1, "Press 'd' to go to next trial     ");
                mvwprintw(info_window, 2, 1, "Press 'x' to quit");
            }
            else
            {
                wattron(info_window, COLOR_PAIR(4));
                mvwprintw(info_window, 1, 1, "All trials completed!             ");
                wattroff(info_window, COLOR_PAIR(4));
                mvwprintw(info_window, 2, 1, "Press 'x' to quit");
            }
            box(stat_window, 0, 0);
            wrefresh(stat_window);
            box(trial_window, 0, 0);
            wrefresh(trial_window);
            box(info_window, 0, 0);
            wrefresh(info_window);
            refresh();

            while (true)
            {
                input_character = getch();
                if (input_character == 'd')
                {
                    break;
                }
                else if (input_character == 'x')
                {
                    if (trial != trials - 1)
                    {
                        printf("Execution of program terminated by user.\n");
                        return 1;
                    }
                    break;
                }
            }
        }
    }

    percolation_threshold_mean = mean(percolation_thresholds, trials);
    standard_deviation = stddev(percolation_threshold_mean, percolation_thresholds, trials);
    confidence_interval_low = confidence_lo(percolation_threshold_mean, standard_deviation, trials);
    confidence_interval_high = confidence_lo(percolation_threshold_mean, standard_deviation, trials);

    if (visualize)
    {
        delwin(stat_window);
        stat_window = NULL;
        delwin(trial_window);
        trial_window = NULL;
        delwin(info_window);
        info_window = NULL;
    }

    // Print to terminal
    printf("mean%*c = %.010f\n", 19, ' ', percolation_threshold_mean);
    printf("stddev%*c = %.010f\n", 17, ' ', standard_deviation);
    printf("95%% confidence interval = [%.010f, %.010f]\n", confidence_interval_low, confidence_interval_high);

    
    return 0;
}

double mean(double *thresholds, int trials)
{
    double sum = 0;
    for (int i = 0; i < trials; i++)
    {
        sum += thresholds[i];
    }

    return sum / trials;
}

double stddev(double mean, double *thresholds, int trials)
{
    double sum = 0;
    for (int i = 0; i < trials; i++)
    {
        sum += (thresholds[i] - mean) * (thresholds[i] - mean);
    }
    return sqrt(sum / (trials - 1));
}

double confidence_lo(double mean, double standard_deviation, int trials)
{
    return mean - (1.96 * standard_deviation) / sqrt(trials);
}

double confidence_hi(double mean, double standard_deviation, int trials)
{
    return mean + (1.96 * standard_deviation) / sqrt(trials);
}