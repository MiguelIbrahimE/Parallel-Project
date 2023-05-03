#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#define MAX_LINE_LENGTH 100
#define MAX_CHUNK_SIZE 10000

struct Car {
    char make[MAX_LINE_LENGTH];
    char model[MAX_LINE_LENGTH];
    int year;
    int mileage;
    int hp;
    char gear[MAX_LINE_LENGTH];
    char fuel[MAX_LINE_LENGTH];
    int price;
};

void sort_chunk(struct Car *arr, int num_cars) {
    // Sort the array of cars by make using insertion sort
    for (int i = 1; i < num_cars; i++) {
        struct Car temp = arr[i];
        int j = i - 1;
        while (j >= 0 && strcmp(arr[j].make, temp.make) > 0) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}

void merge_files(const char *input_filename1, const char *input_filename2, const char *output_filename) {
    // Merge the two input files into a single output file
    // (using the same logic as the sequential version)
}

void sort_file(const char *input_filename, const char *output_filename, int rank, int num_processes) {
    // Get the size of the input file
    FILE *in = fopen(input_filename, "r");
    if (in == NULL) {
        printf("Error opening input file.\n");
    exit(1);
    }
    fseek(in, 0, SEEK_END);
    long file_size = ftell(in);
    fclose(in);

    // Determine the number of chunks and the size of each chunk
    int num_chunks = num_processes;
    int chunk_size = file_size / (MAX_CHUNK_SIZE * sizeof(struct Car)) / num_chunks;
    int remaining_cars = file_size / sizeof(struct Car) - (num_chunks - 1) * chunk_size * MAX_CHUNK_SIZE;

    // Divide the input file into smaller chunks and distribute them among the MPI processes
    struct Car chunk[MAX_CHUNK_SIZE];
    FILE *in_chunk = fopen(input_filename, "r");
    if (in_chunk == NULL) {
        printf("Error opening input file.\n");
        return;
    }
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, in_chunk); // Skip header line
    fseek(in_chunk, rank * chunk_size * MAX_CHUNK_SIZE * sizeof(struct Car), SEEK_CUR);
    if (rank == num_processes - 1) {
        chunk_size = remaining_cars / MAX_CHUNK_SIZE;
    }
    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < MAX_CHUNK_SIZE; j++) {
            fgets(line, MAX_LINE_LENGTH, in_chunk);
            char *token = strtok(line, ",");
            strcpy(chunk[j].make, token);

            token = strtok(NULL, ",");
            strcpy(chunk[j].model, token);

            token = strtok(NULL, ",");
            chunk[j].year = atoi(token);

            token = strtok(NULL, ",");
            chunk[j].mileage = atoi(token);

            token = strtok(NULL, ",");
            chunk[j].hp = atoi(token);

            token = strtok(NULL, ",");
            strcpy(chunk[j].gear, token);

            token = strtok(NULL, ",");
            strcpy(chunk[j].fuel, token);

            token = strtok(NULL, ",");
            chunk[j].price = atoi(token);
        }
        sort_chunk(chunk, MAX_CHUNK_SIZE);
        char output_chunk_filename[MAX_LINE_LENGTH];
        sprintf(output_chunk_filename , "temp_chunk_%d_%d.txt", rank, i);
    FILE *out_chunk = fopen(output_chunk_filename, "w");
    if (out_chunk == NULL) {
        printf("Error opening output file.\n");
        return;
    }
    fprintf(out_chunk, "make,model,year,mileage,hp,gear,fuel,price\n");
    for (int j = 0; j < MAX_CHUNK_SIZE; j++) {
        fprintf(out_chunk, "%s,%s,%d,%d,%d,%s,%s,%d\n", chunk[j].make, chunk[j].model,
            chunk[j].year, chunk[j].mileage, chunk[j].hp, chunk[j].gear, chunk[j].fuel, chunk[j].price);
    }
    fclose(out_chunk);
}
fclose(in_chunk);

// Merge the sorted chunks into a single output file using a k-way merge algorithm
char output_filename1[MAX_LINE_LENGTH];
char output_filename2[MAX_LINE_LENGTH];
char temp_output_filename[MAX_LINE_LENGTH];
for (int i = 0; i < num_chunks; i++) {
    if (rank == i) {
        sprintf(output_filename1, "temp_chunk_%d_%d.txt", rank, i);
        sprintf(output_filename2, "temp_chunk_%d_%d.txt", rank, i + 1);
        break;
    }
}
for (int i = 0; i < num_chunks - 1; i++) {
    if (i % num_processes == rank) {
        sprintf(temp_output_filename, "temp_chunk_%d_%d.txt", rank, i / num_processes + num_chunks);
        merge_files(output_filename1, output_filename2, temp_output_filename);
        remove(output_filename1);
        remove(output_filename2);
        strcpy(output_filename1, temp_output_filename);
        for (int j = 0; j < num_processes; j++) {
            if (j != rank && i + j + 1 < num_chunks) {
                sprintf(output_filename2, "temp_chunk_%d_%d.txt", j, i + j + 1);
                MPI_Send(output_filename1, MAX_LINE_LENGTH, MPI_CHAR, j, 0, MPI_COMM_WORLD);
                MPI_Send(output_filename2, MAX_LINE_LENGTH, MPI_CHAR, j, 0, MPI_COMM_WORLD);
                MPI_Send(temp_output_filename, MAX_LINE_LENGTH, MPI_CHAR, j, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        MPI_Recv(output_filename1, MAX_LINE_LENGTH, MPI_CHAR, i % num_processes, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(output_filename2, MAX_LINE_LENGTH, MPI_CHAR, i % num_processes, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(temp_output_filename, MAX_LINE_LENGTH, MPI_CHAR, i % num_processes, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        merge_files(output_filename1, output_filename2, temp_output_filename);
        remove(output_filename1);
        remove(output_filename2);
        strcpy(output_filename1, temp_output_filename);
    }
}
if (rank == 0) {
    remove(output_filename);
    rename(output_filename1, output_filename);
}
}

int main(int argc, char **argv) {
int rank, num_processes;
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
if (rank == 0) {
    clock_t start_time = clock();
    sort_file("Cars.txt", "output.txt", rank, num_processes);
    clock_t end_time    = clock();
    double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Sorting took %f seconds.\n", total_time);
} else {
    sort_file(NULL, NULL, rank, num_processes);
}

MPI_Finalize();
return 0;
}