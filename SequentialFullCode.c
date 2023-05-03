#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include <math.h>

#define MAX_LINE_LENGTH 100
#define MAX_CHUNK_SIZE 10000
#define MAX_NUM_CARS 150000
#define MAX_VERTICES 100000
#define INF 1e9

int n, m; // number of vertices and edges
int adj[MAX_VERTICES][MAX_VERTICES]; // adjacency matrix
float accl1, accl2, top_speed1, top_speed2; // acceleration and top speed of the two cars
int start_vertex, end_vertex; // indices of the starting and ending vertices
float dist[MAX_VERTICES];
int visited[MAX_VERTICES];

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

void dijkstra() {
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        visited[i] = 0;
    }
    dist[start_vertex] = 0;

    priority_queue<pair<float, int>, vector<pair<float, int>>, greater<pair<float, int>>> pq;
    pq.push({0, start_vertex});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) continue;
        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (adj[u][v] == 0) continue; // no edge between u and v
            float weight = calculate_weight(u, v); // calculate the weight of the edge based on the cars' performance
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
}

float calculate_weight(int u, int v) {
    float distance = sqrt(pow(u - v, 2)); // calculate the Euclidean distance between u and v
    float time1 = calculate_time(distance, accl1, top_speed1); // calculate the time it takes for car 1 to travel the distance
    float time2 = calculate_time(distance, accl2, top_speed2); // calculate the time it takes for car 2 to travel the distance
    float speed1 = calculate_speed(time1, accl1); // calculate the average speed of car 1 during the trip
    float speed2 = calculate_speed(time2, accl2); // calculate the average speed of car 2 during the trip
    float weight = distance / fmin(speed1, speed2); // calculate the weight of the edge based on the slower car's speed
    return weight;
}
float calculate_time(float distance, float acceleration, float top_speed) {
    float time_to_top_speed = top_speed / acceleration; // calculate the time it takes to reach the top speed
    if (distance <= 0.5 * acceleration * pow(time_to_top_speed, 2)) {
    // the car can reach its top speed before reaching halfway point
    return sqrt((2 * distance) / acceleration);
    } else {
    // the car reaches halfway point before reaching its top speed
    float time_to_halfway = time_to_top_speed / 2;
    float distance_to_halfway = 0.5 * acceleration * pow(time_to_halfway, 2);
    float distance_remaining = distance - distance_to_halfway;
    float time_at_top_speed = distance_remaining / top_speed;
    return time_to_halfway + time_at_top_speed + time_to_halfway;
    }
}

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
}
int main() {
clock_t start_time = clock();
sort_file("Cars.txt", "output.txt");

clock_t end_time = clock();
double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
printf("Sorting took %f seconds.\n", total_time);

FILE *fp;
char line[1000];
char make[50], model[50], gear[50], fuel[50];
int year, mileage, hp, price;

fp = fopen("output.txt", "r");
if (fp == NULL) {
    printf("Error opening file\n");
    exit(1);
}

while (fgets(line, 1000, fp) != NULL) {
    sscanf(line, "%[^,],%[^,],%d,%d,%d,%[^,],%[^,],%d", make, model, &year, &mileage, &hp, gear, fuel, &price);
    // do something with the variables
}

fclose(fp);
FILE *fp_in, *fp_out;
char line[1000], make[50], model[50];
int year, mileage, hp, price;
float accl, top_speed;
int found = 0;

fp_in = fopen("SortedCars.txt", "r");
if (fp_in == NULL) {
    printf("Error opening file\n");
    exit(1);
}

fp_out = fopen("Selected.txt", "w");
if (fp_out == NULL) {
    printf("Error creating file\n");
    exit(1);
}

printf("Enter make and model of car 1: ");
scanf("%s %s", make, model);

while (fgets(line, 1000, fp_in) != NULL && found < 2) {
    sscanf(line, "%[^,],%[^,],%d,%d,%d,%*[^,],%*[^,],%d,%f,%f", make, model, &year, &mileage, &hp, &price, &accl, &top_speed);
    if (strcmp(make, input_make) == 0 && strcmp(model, input_model) == 0) {
        found++;
        fprintf(fp_out, "%s", line);
    }
}

printf("Enter make and model of car 2: ");
scanf("%s %s", make, model);

while (fgets(line, 1000, fp_in) != NULL && found < 2) {
    sscanf(line, "%[^,],%[^,],%d,%d,%d,%*[^,],%*[^,],%d,%f,%f", make, model, &year, &mileage, &hp, &price, &accl, &top_speed);
    if (strcmp(make, input_make) == 0 && strcmp(model, input_model) == 0) {
        found++;
        fprintf(fp_out, "%s", line);
    }
}

fclose(fp_in);
fclose(fp_out);

return 0;

}
