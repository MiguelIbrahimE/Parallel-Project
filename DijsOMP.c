#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <omp.h>

#define V 5
int graph[V][V];

int minDistance(int dist[], int sptSet[])
{
    int min = INT_MAX, min_index;

#pragma omp parallel for
    for (int v = 0; v < V; v++)
        if (sptSet[v] == 0 && dist[v] <= min)
#pragma omp critical
        {
            if (dist[v] <= min)
            {
                min = dist[v];
                min_index = v;
            }
        }

    return min_index;
}

void printSolution(int dist[])
{
    printf("Vertex \t Distance from Source\n");
    for (int i = 0; i < V; i++)
        printf("%d \t %d\n", i, dist[i]);
}

void dijkstra(int src)
{
    int dist[V], sptSet[V];

#pragma omp parallel for
    for (int i = 0; i < V; i++)
        dist[i] = INT_MAX, sptSet[i] = 0;

    dist[src] = 0;

#pragma omp parallel for
    for (int count = 0; count < V - 1; count++) {
        int u = minDistance(dist, sptSet);

        sptSet[u] = 1;

#pragma omp parallel for
        for (int v = 0; v < V; v++)
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
                && dist[u] + graph[u][v] < dist[v])
#pragma omp critical
                {
                    if (dist[u] + graph[u][v] < dist[v])
                    {
                        dist[v] = dist[u] + graph[u][v];
                    }
                }
    }

    printSolution(dist);
}

int main()
{
    double start_time = omp_get_wtime();
    // Generate random graph with 1000 vertices and 10000 edges
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j)
                graph[i][j] = 0;
            else if (rand() % 10 == 0)
                graph[i][j] = rand() % 1000;
            else
                graph[i][j] = 0;
        }
    }

    // Measure the start time
    

    // Run Dijkstra's algorithm for vertex 0
    dijkstra(0);

    // Measure the end time
    double end_time = omp_get_wtime();

    // Calculate the time taken
    double time_taken = end_time - start_time;

    printf("Dijkstra's algorithm took %f seconds to execute.\n", time_taken);

    return 0;
}
