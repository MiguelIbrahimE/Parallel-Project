#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double start_time = MPI_Wtime();
    FILE *input_file, *output_file;
    char ch;

    // Open the input file in read mode
    input_file = fopen("Cars.txt", "r");

    // Check if input file opened successfully
    if (input_file == NULL) {
        printf("Error opening input file.\n");
        MPI_Finalize();
        return 1;
    }

    // Open the output file in write mode
    output_file = fopen("output.txt", "w");

    // Check if output file opened successfully
    if (output_file == NULL) {
        printf("Error opening output file.\n");
        MPI_Finalize();
        return 1;
    }

    // Seek to the start position for this rank
    fseek(input_file, rank, SEEK_SET);

    // Read from the input file character by character and write to the output file
    while ((ch = fgetc(input_file)) != EOF) {
        fputc(ch, output_file);
        fseek(input_file, size - 1, SEEK_CUR);
    }

    // Close the files
    fclose(input_file);
    fclose(output_file);
    double end_time = MPI_Wtime();

    // Calculate the time taken
    double time_taken = end_time - start_time;

    // Print the time taken by each process
    printf("Process %d took %lf seconds.\n", rank, time_taken);

    MPI_Finalize();

    return 0;
}
