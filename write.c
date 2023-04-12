#include <stdio.h>
#include <time.h>

int main() {
    FILE *input_file, *output_file;
    char ch;
   clock_t start_time = clock();
    // Open the input file in read mode
    input_file = fopen("Cars.txt", "r");

    // Check if input file opened successfully
    if (input_file == NULL) {
        printf("Error opening input file.\n");
        return 1;
    }

    // Open the output file in write mode
    output_file = fopen("output.txt", "w");

    // Check if output file opened successfully
    if (output_file == NULL) {
        printf("Error opening output file.\n");
        return 1;
    }

    // Measure the start time
   

    // Read from the input file character by character and write to the output file
    while ((ch = fgetc(input_file)) != EOF) {
        fputc(ch, output_file);
    }

    // Measure the end time
   

    // Close the files
    fclose(input_file);
    fclose(output_file);
 clock_t end_time = clock();
    // Calculate the time taken
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("File copied successfully in %lf seconds.\n", time_taken);

    return 0;
}