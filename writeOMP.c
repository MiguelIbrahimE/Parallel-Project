#include <stdio.h>
#include <omp.h>

int main() {
    FILE *input_file, *output_file;
    char ch;
    double start_time, end_time;
    start_time = omp_get_wtime();
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
    

    // Set the number of threads to use
    #pragma omp parallel num_threads(2)
    {
        // Get the ID of the current thread
        int thread_id = omp_get_thread_num();

        // Calculate the start and end positions for this thread
        long start_pos = thread_id * 100000;
        long end_pos = start_pos + 100000;

        // Seek to the start position for this thread
        fseek(input_file, start_pos, SEEK_SET);

        // Read from the input file character by character and write to the output file
        while ((ch = fgetc(input_file)) != EOF && ftell(input_file) < end_pos) {
            fputc(ch, output_file);
        }
    }

    // Measure the end time
    

    // Close the files
    fclose(input_file);
    fclose(output_file);
    end_time = omp_get_wtime();
    // Calculate the time taken
    double time_taken = end_time - start_time;

    printf("File copied successfully in %lf seconds.\n", time_taken);

    return 0;
}