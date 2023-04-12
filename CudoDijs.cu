#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <cuda_runtime.h>

#define V 20000

__global__ void dijkstra_kernel(int* graph, int* dist, int* sptSet, int src)
{
    int tid = threadIdx.x + blockIdx.x * blockDim.x;

    if (tid == src) {
        dist[tid] = 0;
        sptSet[tid] = 1;
    }
    else {
        dist[tid] = INT_MAX;
        sptSet[tid] = 0;
    }

    __syncthreads();

    for (int count = 0; count < V - 1; count++) {
        int u = -1;
        int minDist = INT_MAX;

        for (int v = 0; v < V; v++) {
            if (sptSet[v] == 0 && dist[v] < minDist) {
                u = v;
                minDist = dist[v];
            }
        }

        sptSet[u] = 1;

        for (int v = 0; v < V; v++) {
            int weight = graph[u * V + v];
            if (weight && !sptSet[v] && dist[u] != INT_MAX && dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
            }
        }

        __syncthreads();
    }
}

void printSolution(int dist[])
{
    printf("Vertex \t Distance from Source\n");
    for (int i = 0; i < V; i++)
        printf("%d \t %d\n", i, dist[i]);
}

int main()
{
    int* graph_h = (int*)malloc(V * V * sizeof(int));
    int* dist_h = (int*)malloc(V * sizeof(int));
    int* sptSet_h = (int*)malloc(V * sizeof(int));

    // Generate random graph with 1000 vertices and 10000 edges
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j)
                graph_h[i * V + j] = 0;
            else if (rand() % 10 == 0)
                graph_h[i * V + j] = rand() % 1000;
            else
                graph_h[i * V + j] = 0;
        }
    }

    int* graph_d, *dist_d, *sptSet_d;
    cudaMalloc(&graph_d, V * V * sizeof(int));
    cudaMalloc(&dist_d, V * sizeof(int));
    cudaMalloc(&sptSet_d, V * sizeof(int));

    cudaMemcpy(graph_d, graph_h, V * V * sizeof(int), cudaMemcpyHostToDevice);

    // Measure the start time
    clock_t start_time = clock();

    dijkstra_kernel<<<1, V>>>(graph_d, dist_d, sptSet_d, 0);

    cudaMemcpy(dist_h, dist_d, V * sizeof(int), cudaMemcpyDeviceToHost);

    // Measure the end time
    clock_t end_time = clock();

    // Calculate the time taken
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printSolution(dist_h);
    printf("Dijkstra's algorithm took %f seconds to execute.\n", time_taken);

    cudaFree(graph_d);
    cudaFree(dist_d);
    cudaFree(sptSet_d);

    free(graph_h);
    free(dist_h);
    free(sptSet_h);
    return 0;
    }
