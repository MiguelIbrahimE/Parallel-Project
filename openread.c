#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

int main() {
    FILE *fp_in, *fp_out;
    char *buffer;
    long file_size;
    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock(); // Record the start time
    fp_in = fopen("Cars.txt", "rb"); // Open the input file in binary mode
    if (fp_in == NULL) {
        perror("Failed to open input file");
        exit(1);
    }
    fp_out = fopen("output.txt", "w"); // Open the output file in write mode
    if (fp_out == NULL) {
        perror("Failed to open output file");
        exit(1);
    }

    // Get the size of the file
    fseek(fp_in, 0L, SEEK_END);
    file_size = ftell(fp_in);
    rewind(fp_in);

    buffer = (char*) malloc(file_size); // Allocate a buffer to hold the file data
    if (buffer == NULL) {
        perror("Failed to allocate buffer");
        exit(1);
    }

    // Read the file data in parallel using OpenMP
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nthreads = omp_get_num_threads();
        long start = tid * (file_size / nthreads);
        long end = (tid + 1) * (file_size / nthreads);
        if (tid == nthreads - 1) {
            end = file_size;
        }

        fseek(fp_in, start, SEEK_SET); // Move the file pointer to the start of the current thread's chunk
        fread(&buffer[start], sizeof(char), end - start, fp_in); // Read the data into the buffer
    }

    // Write the buffer contents to the output file in string format
    fprintf(fp_out, "%s", buffer);

    fclose(fp_in); // Close the input file
    fclose(fp_out); // Close the output file

    // Do something with the file data here

    free(buffer); // Free the buffer

    end_time = clock(); // Record the end time
    cpu_time_used = ((double) (end_time - start_time)) / CLOCKS_PER_SEC; // Calculate the time difference in seconds
    printf("CPU time used: %f seconds\n", cpu_time_used);

    return 0;
}
