#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

// Global Variables
#define MAXN 1005
int N, M, start_vertex, end_vertex;
int top_speed1[MAXN], accl1[MAXN], top_speed2[MAXN], accl2[MAXN];
int dist[MAXN];
bool visited[MAXN];
int adjList[MAXN][MAXN][2];
const int INF = 1e9;

float calculate_speed(float time, float acceleration) {
return time * acceleration;
}

float calculate_time(float distance, float acceleration, float top_speed) {
float time_to_reach_top_speed = top_speed / acceleration;
float distance_to_reach_top_speed = calculate_speed(time_to_reach_top_speed, acceleration) * time_to_reach_top_speed / 2;
if (distance_to_reach_top_speed >= distance) {
return sqrtf(2 * distance / acceleration);
}
float remaining_distance = distance - distance_to_reach_top_speed;
return time_to_reach_top_speed + remaining_distance / top_speed;
}

float calculate_weight(int u, int v) {
int distance = adjList[u][v][1];
float time1 = calculate_time(distance, accl1[u], top_speed1[u]);
float time2 = calculate_time(distance, accl2[u], top_speed2[u]);
float speed1 = calculate_speed(time1, accl1[u]);
float speed2 = calculate_speed(time2, accl2[u]);
return fmaxf(time1, time2) + fabsf(speed1 - speed2) / 2;
}

void dijkstra() {
memset(dist, INF, sizeof(dist));
dist[start_vertex] = 0;
for (int i = 0; i < N; i++) {
int u = -1;
for (int j = 0; j < N; j++) {
if (!visited[j] && (u == -1 || dist[j] < dist[u])) {
u = j;
}
}
if (dist[u] == INF) {
break;
}
visited[u] = true;
for (int j = 0; j < N; j++) {
if (adjList[u][j][0] != 0) {
int v = adjList[u][j][0];
int weight = calculate_weight(u, j);
if (dist[v] > dist[u] + weight) {
dist[v] = dist[u] + weight;
}
}
}
}
}

int main() {
scanf("%d %d", &N, &M);
scanf("%d %d", &start_vertex, &end_vertex);
for (int i = 0; i < N; i++) {
    scanf("%d %d %d %d", &top_speed1[i], &accl1[i], &top_speed2[i], &accl2[i]);
}

for (int i = 0; i < M; i++) {
    int u, v, d;
    scanf("%d %d %d", &u, &v, &d);
    adjList[u][v][0] = adjList[v][u][0] = v;
    adjList[u][v][1] = adjList[v][u][1] = d;
}

dijkstra();

printf("Shortest time to reach destination: %.2f\n", dist[end_vertex] / 60.0);

return 0;
}