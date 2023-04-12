#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <mpi.h>

#define V 5

int graph[V][V];

int minDistance(int dist[], int sptSet[], int n)
{
    int min = INT_MAX, min_index;

    for (int v = 0; v < n; v++)
        if (sptSet[v] == 0 && dist[v] <= min)
            min = dist[v], min_index = v;

    return min_index;
}

void printSolution(int dist[], int n)
{
    printf("Vertex \t Distance from Source\n");
    for (int i = 0; i < n; i++)
        printf("%d \t %d\n", i, dist[i]);
}

void dijkstra(int src, int n, int rank, int size)
{
    int dist[n], sptSet[n];

    for (int i = 0; i < n; i++)
        dist[i] = INT_MAX, sptSet[i] = 0;

    dist[src] = 0;

    for (int count = 0; count < n - 1; count++) {
        int u = minDistance(dist, sptSet, n);

        sptSet[u] = 1;

        int local_chunk_size = n / size;
        int local_chunk_start = rank * local_chunk_size;
        int local_chunk_end = local_chunk_start + local_chunk_size;

        if (rank == size - 1) {
            local_chunk_end = n;
        }

        for (int v = local_chunk_start; v < local_chunk_end; v++) {
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
                && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }

        int *all_dist = (int *)malloc(n * sizeof(int));
        MPI_Allgather(dist + local_chunk_start, local_chunk_size, MPI_INT, all_dist, local_chunk_size, MPI_INT, MPI_COMM_WORLD);

        for (int i = 0; i < n; i++) {
            if (dist[i] > all_dist[i]) {
                dist[i] = all_dist[i];
            }
        }

        free(all_dist);
    }

    if (rank == 0) {
        printSolution(dist, n);
    }
}

int main(int argc, char *argv[])
{
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double start_time = MPI_Wtime();
    // Generate random graph with V vertices and 10% density
    srand(time(NULL));

    int local_chunk_size = V / size;
    int local_chunk_start = rank * local_chunk_size;
    int local_chunk_end = local_chunk_start + local_chunk_size;

    if (rank == size - 1) {
        local_chunk_end = V;
    }

    for (int i = 0; i < local_chunk_size; i++) {
        for (int j = 0; j < V; j++) {
            if (local_chunk_start + i == j) {
                graph[local_chunk_start + i][j] = 0;
            } else if (rand() % 10 == 0) {
                graph[local_chunk_start + i][j] = rand() % 1000;
            } else {
                graph[local_chunk_start + i][local_chunk_start + j] = 0;
            }
        }
    }
   

dijkstra(0, V, rank, size);

double end_time = MPI_Wtime();

if (rank == 0) {
    printf("Time taken: %f seconds\n", end_time - start_time);
}

MPI_Finalize();

return 0;
}
