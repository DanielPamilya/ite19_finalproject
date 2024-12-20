#include <stdio.h>  // Includes standard input-output functions.
#include <stdlib.h> // Includes functions for memory allocation, process control, etc.
#include <string.h> // Includes functions for handling strings.
#include <ctype.h>  // Includes functions for character classification and conversion.

// Function declarations
int romanToDecimal(const char *roman);                // Converts Roman numerals to decimal numbers.
void decimalToWords(int num, char *result);           // Converts decimal numbers to their word representation.
int performOperation(int num1, int num2, char op);    // Performs basic arithmetic operations.
void processLine(const char *line, FILE *outputFile); // Processes a line from the input file and writes the output.
void sanitizeInput(char *line);                       // Cleans up input by removing unwanted characters.

// Main function
int main()
{
    FILE *inputFile = fopen("input.txt", "r");   // Opens the input file for reading.
    FILE *outputFile = fopen("output.txt", "w"); // Opens the output file for writing.

    if (!inputFile || !outputFile) // Checks if either file failed to open.
    {
        printf("Error: Unable to open file.\n"); // Prints an error message if files can't be opened.
        return 1;                                // Exits the program with an error code.
    }

    char line[256];                              // Buffer for storing each line from the input file.
    while (fgets(line, sizeof(line), inputFile)) // Reads each line from the input file.
    {
        sanitizeInput(line);           // Removes unwanted characters from the input line.
        processLine(line, outputFile); // Processes the sanitized line and writes to the output file.
    }

    fclose(inputFile);  // Closes the input file.
    fclose(outputFile); // Closes the output file.

    printf("Processing complete. Check output.txt for results.\n"); // Informs the user that processing is done.
    return 0;                                                       // Exits the program successfully.
}

// Convert Roman numeral to decimal
int romanToDecimal(const char *roman)
{
    int values[26] = {0};     // Initializes an array to map Roman numeral letters to their values.
    values['I' - 'A'] = 1;    // Maps 'I' to 1.
    values['V' - 'A'] = 5;    // Maps 'V' to 5.
    values['X' - 'A'] = 10;   // Maps 'X' to 10.
    values['L' - 'A'] = 50;   // Maps 'L' to 50.
    values['C' - 'A'] = 100;  // Maps 'C' to 100.
    values['D' - 'A'] = 500;  // Maps 'D' to 500.
    values['M' - 'A'] = 1000; // Maps 'M' to 1000.

    int total = 0;                               // Accumulates the final decimal value.
    int prevValue = 0;                           // Stores the value of the previous Roman numeral for comparison.
    for (int i = strlen(roman) - 1; i >= 0; i--) // Iterates through the Roman numeral string in reverse.
    {
        int value = values[roman[i] - 'A']; // Gets the value of the current Roman numeral.
        if (value < prevValue)              // If the current value is less than the previous, subtract it.
        {
            total -= value;
        }
        else // Otherwise, add the value.
        {
            total += value;
        }
        prevValue = value; // Updates the previous value.
    }
    return total; // Returns the total decimal value.
}

// Convert decimal number to words
void decimalToWords(int num, char *result)
{
    // Arrays storing words for ones, teens, tens, and thousands.
    const char *ones[] = {"", "One", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine"};
    const char *teens[] = {"Ten", "Eleven", "Twelve", "Thirteen", "Fourteen", "Fifteen", "Sixteen", "Seventeen", "Eighteen", "Nineteen"};
    const char *tens[] = {"", "", "Twenty", "Thirty", "Forty", "Fifty", "Sixty", "Seventy", "Eighty", "Ninety"};
    const char *thousands[] = {"", "Thousand", "Million", "Billion"};

    if (num == 0) // Handles the special case where the number is 0.
    {
        strcpy(result, "Zero");
        return;
    }

    char buffer[1024] = ""; // Buffer for constructing the word representation.
    int place = 0;          // Tracks the position in the thousands array.

    while (num > 0) // Processes the number in chunks of three digits.
    {
        int chunk = num % 1000; // Extracts the last three digits.
        if (chunk > 0)          // If the chunk is non-zero, convert it to words.
        {
            char chunkBuffer[256] = "";  // Buffer for the current chunk.
            int hundred = chunk / 100;   // Extracts the hundreds digit.
            int remainder = chunk % 100; // Gets the remainder after removing the hundreds.

            if (hundred > 0) // If there is a hundreds digit, add it to the buffer.
            {
                strcat(chunkBuffer, ones[hundred]);
                strcat(chunkBuffer, " Hundred ");
            }

            if (remainder >= 10 && remainder <= 19) // Handles teen numbers.
            {
                strcat(chunkBuffer, teens[remainder - 10]);
            }
            else // Handles tens and ones separately.
            {
                int ten = remainder / 10; // Extracts the tens digit.
                int one = remainder % 10; // Extracts the ones digit.

                if (ten > 0)
                    strcat(chunkBuffer, tens[ten]); // Adds the tens digit to the buffer.
                if (ten > 0 && one > 0)
                    strcat(chunkBuffer, " "); // Adds a space if both tens and ones exist.
                if (one > 0)
                    strcat(chunkBuffer, ones[one]); // Adds the ones digit to the buffer.
            }

            if (place > 0) // Adds the corresponding thousands place if applicable.
            {
                strcat(chunkBuffer, " ");
                strcat(chunkBuffer, thousands[place]);
            }

            if (strlen(buffer) > 0)
                strcat(chunkBuffer, " "); // Adds a space between chunks.
            strcat(chunkBuffer, buffer);  // Appends the current chunk to the buffer.
            strcpy(buffer, chunkBuffer);  // Copies the chunk back to the main buffer.
        }

        num /= 1000; // Moves to the next chunk.
        place++;     // Increments the thousands place.
    }

    strcpy(result, buffer); // Copies the final result to the output.
}

// Perform arithmetic operation
int performOperation(int num1, int num2, char op)
{
    switch (op) // Determines the operation based on the operator.
    {
    case '+': // Addition
        return num1 + num2;
    case '-':                    // Subtraction
        return abs(num1 - num2); // Ensures the result is non-negative.
    case '*':                    // Multiplication
        return num1 * num2;
    case '/':                               // Division
        return num2 != 0 ? num1 / num2 : 0; // Handles division by zero.
    default:                                // Unknown operator
        return 0;
    }
}

// Process a line from the input file
void processLine(const char *line, FILE *outputFile)
{
    char roman1[64], roman2[64], op;                        // Buffers for the two Roman numerals and the operator.
    if (sscanf(line, "%s %c %s", roman1, &op, roman2) != 3) // Parses the line into its components.
    {
        fprintf(outputFile, "Invalid input format\n"); // Writes an error message if the format is invalid.
        return;
    }

    int num1 = romanToDecimal(roman1);             // Converts the first Roman numeral to decimal.
    int num2 = romanToDecimal(roman2);             // Converts the second Roman numeral to decimal.
    int result = performOperation(num1, num2, op); // Performs the arithmetic operation.

    char resultWords[1024];              // Buffer for the result in words.
    decimalToWords(result, resultWords); // Converts the result to words.

    fprintf(outputFile, "%s\n", resultWords); // Writes the result to the output file.
}

// Sanitize input line by removing unwanted characters
void sanitizeInput(char *line)
{
    char *dst = line;                           // Destination pointer for sanitized output.
    for (char *src = line; *src != '\0'; src++) // Iterates through the input line.
    {
        if (isalnum(*src) || *src == ' ' || *src == '+' || *src == '-' || *src == '*' || *src == '/') // Keeps only valid characters.
        {
            *dst++ = *src; // Copies the valid character to the destination.
        }
    }
    *dst = '\0'; // Null-terminates the sanitized string.
}
