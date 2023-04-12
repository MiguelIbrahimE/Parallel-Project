#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 100
#define MAX_CHUNK_SIZE 10000
#define MAX_NUM_CARS 150000

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

void merge_files(FILE *f1, FILE *f2, const char *output_filename) {
    FILE *out = fopen(output_filename, "w");
    if (out == NULL) {
        printf("Error opening output file.\n");
        return;
    }
    char line1[MAX_LINE_LENGTH];
    char line2[MAX_LINE_LENGTH];
    fgets(line1, MAX_LINE_LENGTH, f1);
    fgets(line2, MAX_LINE_LENGTH, f2);
    while (!feof(f1) && !feof(f2)) {
        if (strcmp(line1, line2) < 0) {
            fputs(line1, out);
            fgets(line1, MAX_LINE_LENGTH, f1);
        } else {
            fputs(line2, out);
            fgets(line2, MAX_LINE_LENGTH, f2);
        }
    }
    while (!feof(f1)) {
        fputs(line1, out);
        fgets(line1, MAX_LINE_LENGTH, f1);
    }
    while (!feof(f2)) {
        fputs(line2, out);
        fgets(line2, MAX_LINE_LENGTH, f2);
    }
    fclose(out);
}

void sort_chunk(const char *input_filename, const char *output_filename) {
    struct Car arr[MAX_CHUNK_SIZE];
    FILE *in = fopen(input_filename, "r");
    if (in == NULL) {
        printf("Error opening input file.\n");
        return;
    }
    int num_cars = 0;
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, in); // Skip header line
    while (fgets(line, MAX_LINE_LENGTH, in) != NULL && num_cars < MAX_CHUNK_SIZE) {
        char *token = strtok(line, ",");
        strcpy(arr[num_cars].make, token);

        token = strtok(NULL, ",");
        strcpy(arr[num_cars].model, token);

        token = strtok(NULL, ",");
        arr[num_cars].year = atoi(token);

        token = strtok(NULL, ",");
        arr[num_cars].mileage = atoi(token);

        token = strtok(NULL, ",");
        arr[num_cars].hp = atoi(token);

        token = strtok(NULL, ",");
        strcpy(arr[num_cars].gear, token);

        token = strtok(NULL, ",");
        strcpy(arr[num_cars].fuel, token);

        token = strtok(NULL, ",");
        arr[num_cars].price = atoi(token);

        num_cars++;
    }
    fclose(in);

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

    // Write the sorted chunk to a temporary file
    FILE *out = fopen(output_filename, "w");
    if (out == NULL) {
        printf("Error opening output file.\n");
        return;
    }
    // Continue writing the output file
    for (int i = 0; i < num_cars; i++) {
        fprintf(out, "%s,%s,%d,%d,%d,%s,%s,%d\n", arr[i].make, arr[i].model, arr[i].year,
        arr[i].mileage, arr[i].hp, arr[i].gear, arr[i].fuel, arr[i].price);
    }
fclose(out);
}

void sort_file(const char *input_filename, const char *output_filename) {
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
int num_chunks = file_size / (MAX_NUM_CARS * sizeof(struct Car)) + 1;
int chunk_size = MAX_NUM_CARS / num_chunks;

// Sort each chunk and write it to a temporary file
char chunk_filenames[num_chunks][MAX_LINE_LENGTH];
for (int i = 0; i < num_chunks; i++) {
    char output_chunk_filename[MAX_LINE_LENGTH];
    sprintf(output_chunk_filename, "temp_chunk_%d.txt", i);
    sort_chunk(input_filename, output_chunk_filename);
    strcpy(chunk_filenames[i], output_chunk_filename);
}

// Merge the sorted chunks into a single output file
FILE *out = fopen(output_filename, "w");
if (out == NULL) {
    printf("Error opening output file.\n");
    return;
}
FILE *chunk_files[num_chunks];
for (int i = 0; i < num_chunks; i++) {
    chunk_files[i] = fopen(chunk_filenames[i], "r");
    if (chunk_files[i] == NULL) {
        printf("Error opening chunk file.\n");
        return;
    }
}
int num_cars[num_chunks];
for (int i = 0; i < num_chunks; i++) {
    num_cars[i] = 0;
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, chunk_files[i]); // Skip header line
    while (fgets(line, MAX_LINE_LENGTH, chunk_files[i]) != NULL) {
        fprintf(out, "%s", line);
        num_cars[i]++;
    }
}
for (int i = 0; i < num_chunks; i++) {
    fclose(chunk_files[i]);
}
fclose(out);

// Delete the temporary chunk files
for (int i = 0; i < num_chunks; i++) {
    remove(chunk_filenames[i]);
}
int main() {
clock_t start_time = clock();
sort_file("cars.txt", "output.txt");

clock_t end_time = clock();
double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
printf("Sorting took %f seconds.\n", total_time);

return 0;

}