/* Dijkstra's two stack algorithm using a generic stack macro 
 * Allows for multiplication, division, addition, and subtraction of floats
 */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "stack.h"

DEFINE_STACK_TYPE(double, numeric);
DEFINE_STACK_TYPE(char, character);

double get_solution(char *operation);
double do_operation(double a, double b, char operator);
// void evaluate_previous(int n, numeric_stack *value_stack, character_stack *operator_stack, bool includes_last);

int main(void)
{
    // Input buffer
    char buffer[1024];

    while (true)
    {
        printf("\nEnter a mathematical operation (type 'q' to quit): ");
        // Get input
        if (fgets(buffer, sizeof(buffer), stdin))
        {
            // Remove newline character
            buffer[strcspn(buffer, "\n")] = 0;
        }

        if (buffer[0] == 'q' && strlen(buffer) == 1)
        {
            printf("Exiting program...\n\n");
            break;
        }

        // Get solution
        double solution = get_solution(buffer);
        printf("Solution: %.10lf\n", solution);
    }

    return 0;
}

double get_solution(char *operation)
{
    // Character parsing variables
    int length = strlen(operation);
    int bracket_depth = 0;
    double current_value = 0.0;
    bool previous_is_digit = false;
    bool previous_is_dot = false;
    int decimal_divider = 0;

    // Math operation variables
    double p;
    double q;
    double r;
    char operator;
    char next_operator;
    double sub_solution;

    // Create value and operator stacks
    numeric_stack *value_stack = stack_numeric_create();
    character_stack *operator_stack = stack_character_create();

    char c;

    for (int i = 0; i < length; i++)
    {
        c = operation[i];

        if (c == ' ')
        {
            assert(!previous_is_dot);
            previous_is_digit = false;
            previous_is_dot = false;
            decimal_divider = 0;
        }
        else if (c == '(')
        {
            assert(!previous_is_dot);
            stack_character_push(operator_stack, c);
            bracket_depth++;
            previous_is_digit = false;
            previous_is_dot = false;
            decimal_divider = 0;
        }
        else if (c == ')')
        {
            assert(!(bracket_depth == 0));
            assert(!previous_is_dot);
            bracket_depth--;

            while (true)
            {
                operator= stack_character_pop(operator_stack);

                // if (operator== '^')
                // {
                //     p = stack_numeric_pop(value_stack);
                //     q = stack_numeric_pop(value_stack);
                //     sub_solution = do_operation(q, p, operator);
                //     stack_numeric_push(value_stack, sub_solution);
                // }
                if (operator== '*' || operator== '/')
                {
                    p = stack_numeric_pop(value_stack);
                    q = stack_numeric_pop(value_stack);
                    sub_solution = do_operation(q, p, operator);
                    stack_numeric_push(value_stack, sub_solution);
                }
                else if (operator== '+' || operator== '-')
                {
                    p = stack_numeric_pop(value_stack);
                    q = stack_numeric_pop(value_stack);
                    next_operator = stack_character_pop(operator_stack);
                    if (next_operator == '*' || next_operator == '/')
                    {
                        r = stack_numeric_pop(value_stack);
                        sub_solution = do_operation(r, q, next_operator);
                        stack_numeric_push(value_stack, sub_solution);
                        stack_numeric_push(value_stack, p);
                        stack_character_push(operator_stack, operator);
                    }
                    else if (next_operator == '+' || next_operator == '-' || next_operator == '(')
                    {
                        stack_character_push(operator_stack, next_operator);
                        sub_solution = do_operation(q, p, operator);
                        stack_numeric_push(value_stack, sub_solution);
                    }
                }
                else if (operator== '(')
                {
                    break;
                }

                previous_is_digit = false;
                previous_is_dot = false;
                decimal_divider = 0;
            }
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            assert(!previous_is_dot);
            stack_character_push(operator_stack, c);
            previous_is_digit = false;
        }
        else if (isdigit(c))
        {
            if (previous_is_dot || decimal_divider > 0)
            {
                current_value += ((double)(c - '0')) / decimal_divider;
                decimal_divider *= 10;
            }
            else
            {
                if (previous_is_digit)
                {
                    current_value *= 10;
                }
                current_value += (double)(c - '0');
            }

            if ((i + 1 == length) || (!isdigit(operation[i + 1]) && !(operation[i + 1] == '.')))
            {
                stack_numeric_push(value_stack, current_value);
                current_value = 0.0;
                decimal_divider = 0;
            }

            previous_is_digit = true;
            previous_is_dot = false;
        }
        else if (c == '.')
        {
            assert(decimal_divider == 0);
            assert(isdigit(operation[i + 1]));

            previous_is_digit = false;
            previous_is_dot = true;
            decimal_divider = 10;
        }
        else
        {
            printf("Warning: invalid input, results not reliable\n");
            return 0;
        }
    }

    assert(bracket_depth == 0);

    while (operator_stack->size != 0 && value_stack->size != 1)
    {
        operator= stack_character_pop(operator_stack);

        if (operator== '*' || operator== '/' || operator_stack->size == 0)
        {
            p = stack_numeric_pop(value_stack);
            q = stack_numeric_pop(value_stack);
            sub_solution = do_operation(q, p, operator);
            stack_numeric_push(value_stack, sub_solution);
        }
        else if (operator== '+' || operator== '-')
        {
            p = stack_numeric_pop(value_stack);
            q = stack_numeric_pop(value_stack);
            next_operator = stack_character_pop(operator_stack);
            if (next_operator == '*' || next_operator == '/')
            {
                r = stack_numeric_pop(value_stack);
                sub_solution = do_operation(r, q, next_operator);
                stack_numeric_push(value_stack, sub_solution);
                stack_numeric_push(value_stack, p);
                stack_character_push(operator_stack, operator);
            }
            else if (next_operator == '+' || next_operator == '-')
            {
                stack_character_push(operator_stack, next_operator);
                sub_solution = do_operation(q, p, operator);
                stack_numeric_push(value_stack, sub_solution);
            }
        }
    }
    double solution = stack_numeric_pop(value_stack);
    stack_numeric_free(value_stack);
    stack_character_free(operator_stack);

    return solution;
}

double do_operation(double a, double b, char operator)
{
    double solution = 0.0;

    if (operator== '+')
    {
        solution = a + b;
    }
    else if (operator== '-')
    {
        solution = a - b;
    }
    else if (operator== '*')
    {
        solution = a * b;
    }
    else if (operator== '/')
    {
        solution = a / b;
    }
    // else if (operator== '^')
    // {
    //     solution == a;
    //     for (int i = 0; i < (b - 1); i++)
    //     {
    //         solution *= a;
    //     }
    // }

    return solution;
}