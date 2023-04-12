#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE_LENGTH 100
#define MAX_NUM_LINES 20000

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

void swap(struct Car *a, struct Car *b) {
    struct Car temp = *a;
    *a = *b;
    *b = temp;
}

int compare_strings(const void *a, const void *b) {
    return strcmp(((struct Car*)a)->make, ((struct Car*)b)->make);
}

int compare_integers(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int compare_cars(const void *a, const void *b) {
    return compare_strings(a, b);
}

void quicksort(struct Car *arr, int left, int right) {
    if (left < right) {
        int pivot_index = left + (right - left) / 2;
        struct Car pivot_value = arr[pivot_index];
        int i = left;
        int j = right;
        while (i <= j) {
            while (compare_cars(&arr[i], &pivot_value) < 0) {
                i++;
            }
            while (compare_cars(&arr[j], &pivot_value) > 0) {
                j--;
            }
            if (i <= j) {
                swap(&arr[i], &arr[j]);
                i++;
                j--;
            }
        }
        quicksort(arr, left, j);
        quicksort(arr, i, right);
    }
}

int read_cars(const char *filename, struct Car *arr, int max_lines) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file '%s'.\n", filename);
        return 0;
    }

    int count = 0;
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, fp); // Skip header line
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL && count < max_lines) {
        char *token = strtok(line, ",");
        strcpy(arr[count].make, token);

        token = strtok(NULL, ",");
        strcpy(arr[count].model, token);

        token = strtok(NULL, ",");
        arr[count].year = atoi(token);

        token = strtok(NULL, ",");
        arr[count].mileage = atoi(token);

        token = strtok(NULL, ",");
        arr[count].hp = atoi(token);

        token = strtok(NULL, ",");
        strcpy(arr[count].gear, token);

        token = strtok(NULL, ",");
        strcpy(arr[count].fuel, token);

        token = strtok(NULL, ",");
        arr[count].price = atoi(token);

        count++;
    }

    fclose(fp);
    return count;
}

int main() {
    clock_t start_time, end_time;
    double time_taken;
    start_time = clock(); // get initial clock time
    
    // Open the file
    FILE *fp = fopen("Cars.txt", "r");
    if (fp == NULL) {
        printf("Error opening file 'Cars.txt'.\n");
        return 1;
    }
    
    // Count the number of lines in the file
    int num_lines = 0;
    char ch = getc(fp);
    while (ch != EOF) {
        if (ch == '\n') {
            num_lines++;
        }
        ch = getc(fp);
    }
    
    // Allocate memory for the array of structs
    struct Car *arr = malloc(num_lines * sizeof(struct Car));
    if (arr == NULL) {
        printf("Error allocating memory.\n");
        return 1;
    }
    
    // Reset the file position indicator to the beginning of the file
    fseek(fp, 0, SEEK_SET);
    
    // Read the data from the file into the array of structs
    int count = 0;
    char line[MAX_LINE_LENGTH];
    fgets(line, MAX_LINE_LENGTH, fp); // Skip header line
    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        char *token = strtok(line, ",");
        strcpy(arr[count].make, token);

        token = strtok(NULL, ",");
        strcpy(arr[count].model, token);

        token = strtok(NULL, ",");
        arr[count].year = atoi(token);

        token = strtok(NULL, ",");
        arr[count].mileage = atoi(token);

        token = strtok(NULL, ",");
        arr[count].hp = atoi(token);

        token = strtok(NULL, ",");
        strcpy(arr[count].gear, token);

        token = strtok(NULL, ",");
        strcpy(arr[count].fuel, token);

        token = strtok(NULL, ",");
        arr[count].price = atoi(token);

        count++;
    }
    
    // Close the file
    fclose(fp);
    
    // Sort the array of structs
    quicksort(arr, 0, num_lines - 1);
    
    // Print the sorted array of structs
    for (int i = 0; i < num_lines; i++) {
        printf("%s, %s, %d, %d, %d, %s, %s, %d\n",
            arr[i].make, arr[i].model, arr[i].year,
            arr[i].mileage, arr[i].hp, arr[i].gear,
            arr[i].fuel, arr[i].price);
    }
    
    // Free the memory allocated for the array of structs
    free(arr);
    
    // Calculate and print the time taken to execute the program
    end_time = clock();
    time_taken = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds.\n", time_taken);
    
    return 0;
}
