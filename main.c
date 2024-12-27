#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

// Komşuluk matrisindeki bağlı olmama durumu
#define INF -1

// Komşuluk matrisi graf yapısı
typedef struct {
  int vertices;
  int* matrix;
} Graph;

// Kenar yapısı, sadece algoritmada kolaylık amaçlı
typedef struct {int u; int v; int w;} Edge;

// Kenarlar üzerinde for döngüsü
// u, v ile kenarlar alınabilir
#define forEdges(g) \
  for (int u = 0; u < (g).vertices; u++)\
  for (int v = u + 1; v < (g).vertices; v++)\
  if (weigth(g, u, v) != INF)\

// Matrisin v'nci satırının u'ncu sütunundaki değer
inline int weigth(Graph g, int u, int v){
  return g.matrix[g.vertices * v + u];
}

// Kenar sayısı
int edgeCount(Graph g) {
  int sum = 0;
  forEdges(g){
    if (weigth(g, u, v) != INF) sum++;
  }
  return sum;
}

// Kenar ağırlıklarının toplamı
int totalEdgeWeight(const Graph g) {
  int sum = 0;
  forEdges(g){
    if (weigth(g, u, v) != INF) sum += weigth(g, u, v);
  }
  return sum;
}

// Graphviz için DOT gösterimini yazdır
int printDOT(const Graph g) {
  printf("graph G {\n");
  int sum = 0;
  forEdges(g){
    if (weigth(g, u, v) != INF)
      printf("%c -- %c [label=%d];\n", 'A' + u, 'A' + v, weigth(g, u, v));
  }
  printf("}\n");
  return sum;
}

// Graf başlangıç değerlerini boş olarak ayarla
void initGraph(Graph* g, int vertexCount) {
  g->vertices = vertexCount;
  g->matrix = malloc(vertexCount * vertexCount * sizeof(int));
  memset(g->matrix, INF, vertexCount * vertexCount * sizeof(int));
}

// Bir Graph yapısının verisini başka bir Graph yapısına kopyala
void copyGraph(const Graph* original, Graph* copy) {
  copy->vertices = original->vertices;
  copy->matrix = malloc(original->vertices * original->vertices * sizeof(int));
  memcpy( copy->matrix
        , original->matrix
        , original->vertices * original->vertices );
}

// u düğümünden v düğümüne, weight ağırlıklı bir kenar ekle
void addEdge(Graph* g, int u, int v, int weight) {
  g->matrix[u*g->vertices + v] = weight;
  g->matrix[v*g->vertices + u] = weight;
}

// u düğümünden v düğümüne olan kenarı sil
void removeEdge(Graph* g, int u, int v) {
  g->matrix[u*g->vertices + v] = INF;
  g->matrix[v*g->vertices + u] = INF;
}

// Ağırlık değeri en yüksek olan grafı bul
void findBiggestEdge(const Graph* g, Edge* e, int* wr) {
  *e = (Edge){0,0};
  forEdges(*g){
    if (weigth(*g, e->u, e->v) < weigth(*g, u, v)){
      e->u = u;
      e->v = v;
    }
  }
  *wr = weigth(*g, e->u, e->v);
}

// Depth First Search ile bulunan toplam düğüm sayısını döndürür
int DFS(Graph* g, int v, bool* flags) {
  flags[v] = true;
  //printf("Visited %d\n", v);
  
  int count = 1;
  for (int w = 0; w < g->vertices; w++) {
    if (!flags[w] && (weigth(*g, v, w) != INF)) 
      count += DFS(g, w, flags);
  }
  return count;
}

// Graf bağlı bir graf mı?
// DFS trafından bulunan düğüm sayısı toplam düğüm sayısına eşit mi?
bool isConnected(Graph* g) {
  bool flags[g->vertices];
  memset(flags, false, g->vertices);

  return g->vertices == DFS(g, 0, flags);
}

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// cond sinyalinin gelmesini bekle
#define WAITFORNEXT \
  pthread_mutex_lock(&lock);\
  pthread_cond_wait(&cond, &lock);\
  pthread_mutex_unlock(&lock)

Edge* edges = NULL;
int edgeIndex = -1;

// Reverse-Delete Algoritmasının işlendiği thread başlangıcı
void* reverseDelete(void* _g) {

  // pthread void* istiyor, ben Graph* istiyorum
  Graph* g = _g;
  edges = malloc(edgeCount(*g) * sizeof(Edge));
  // Matristen kenar listesini çıkar
  int ei = 0;
  forEdges(*g) {
    edges[ei].u = u;
    edges[ei].v = v;
    edges[ei].w = weigth(*g, u, v);
    ei++;
  }
  WAITFORNEXT;

  // Bubble sort ile sırala
  printf("Sorting the edges\n");
  for (int i = 0; i < edgeCount(*g); i++)
  for (int j = i + 1; j < edgeCount(*g); j++) {
    if (edges[i].w < edges[j].w) {
      int temp;
      temp = edges[i].w;
      edges[i].w = edges[j].w;
      edges[j].w = temp;

      temp = edges[i].u;
      edges[i].u = edges[j].u;
      edges[j].u = temp;

      temp = edges[i].v;
      edges[i].v = edges[j].v;
      edges[j].v = temp;
    }
  }
  WAITFORNEXT;
  
  // Algoritma kenarları aktif olarak sildiği için kenar sayısını sabit tutmamız lazım
  int initialEdgeCount = edgeCount(*g);
  for (edgeIndex = 0; edgeIndex < initialEdgeCount; edgeIndex++) {
    if (edgeCount(*g) == (g->vertices - 1)) {
      printf("E = V - 1 koşulu sağlandı.\n");
      break;
    }

    printf("Selecting\n");
    printf( "%d--%d, w:%d\n"
          , edges[edgeIndex].u
          , edges[edgeIndex].v
          , edges[edgeIndex].w );
    
    WAITFORNEXT;
    
    /*
    printf("From\n");
    forEdges(*g) printf("%d--%d: %d\n", u, v, weigth(*g, u, v));
    */
    printf("Removing\n");
    printf("%d--%d\n", edges[edgeIndex].u, edges[edgeIndex].v);
    
    removeEdge(g, edges[edgeIndex].u, edges[edgeIndex].v);

    // Kenarı geri eklememiz gerekirse diye ağırlığı yedekliyoruz
    int backupW = edges[edgeIndex].w;
    edges[edgeIndex].w = INF;
    WAITFORNEXT;

    // Eğer grafın bağlılığını bozduysak kenarı geri ekliyoruz
    if (!isConnected(g)) {
      printf("Graph was broken, add it back.\n");
      edges[edgeIndex].w = backupW;
      addEdge(g, edges[edgeIndex].u, edges[edgeIndex].v, backupW);
      WAITFORNEXT;
    } else {
      printf("No problems, next edge.\n");
    }

  }
  
  WAITFORNEXT;
  
  edgeIndex = -1;
  printDOT(*g);

  return NULL;
}

int main(){
  int windowWidth = 500;
  int windowHeight = 500;
  //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(windowWidth, windowHeight, "Reverse-Delete Algorithm");
  
  Vector2 offset = {-250, -250};
  
  Graph g;
  initGraph(&g, 5);

  addEdge(&g, 0, 1, 2);
  addEdge(&g, 0, 2, 5);
  addEdge(&g, 0, 3, 4);
  addEdge(&g, 1, 2, 12);
  addEdge(&g, 1, 3, 25);
  addEdge(&g, 2, 3, 2);
  addEdge(&g, 3, 4, 10);

  printDOT(g);

  // printf("is connected: %d\n", isConnected(&g));

  pthread_t calcThreadId;
  pthread_create(&calcThreadId, NULL, reverseDelete, &g);

  // Başlangıçta düğümleri çember şeklinde yerleştir
  float radius = 150;
  Vector2 positions[g.vertices];
  for (int i = 0; i < g.vertices; i++) {
    positions[i] = (Vector2){
      .x = radius * sin(i * 2 * PI / g.vertices),
      .y = radius * -cos(i * 2 * PI / g.vertices),
    };
  }

  int initialEdgeCount = edgeCount(g);

  while (!WindowShouldClose()) {

    windowWidth = GetRenderWidth();
    windowHeight = GetRenderHeight();
    
    // Bir sonraki adıma ilerle
    if (IsKeyPressed(KEY_N)) {
      pthread_cond_signal(&cond);
    }

    // Kamera Hareketi
    if (IsKeyDown(KEY_W)) offset.y -= 0.05;
    if (IsKeyDown(KEY_S)) offset.y += 0.05;
    if (IsKeyDown(KEY_A)) offset.x -= 0.05;
    if (IsKeyDown(KEY_D)) offset.x += 0.05;


    BeginDrawing();
    ClearBackground(BLACK);

    // Algoritmanın durum bilgileri
    
    // Kenar listesi
    if (edges != NULL)
    for(int ei = 0; ei < initialEdgeCount; ei++){
      if (edges[ei].w != INF) {
        DrawText(TextFormat("%d", edges[ei].w), 20, ei * 25, 20, WHITE);
        DrawLine(40, ei * 25 + 10, 40 + 10 * edges[ei].w, ei * 25 + 10, WHITE);
      }
    }
    // Şu anda işlediğimiz kenar için imleç
    DrawText(">", 5, edgeIndex * 25, 20, WHITE);
    
    // Graf hala bağlı mı?
    bool connected = isConnected(&g);
    DrawText( connected ? "Connected" : "Disconnected"
            , 20, windowHeight - 50
            , 20
            , connected ? GREEN : RED);
    
    // Grafın toplam ağırlığı
    DrawText( TextFormat("Total Weight: %d", totalEdgeWeight(g))
            , 20, windowHeight - 25, 20, BLUE);


    // Çizim

    // Kenarlar
    forEdges(g){
      Vector2 uPos = Vector2Subtract(positions[u], offset);
      Vector2 vPos = Vector2Subtract(positions[v], offset);
      Vector2 wPos = Vector2Scale(Vector2Add(uPos, vPos), 0.5);

      // İşlediğimiz kenar varsa arkasına bir highlight
      if (edges != NULL)
      if (edgeIndex != -1)
      if (v == edges[edgeIndex].v)
      if (u == edges[edgeIndex].u)
        DrawLineEx(uPos, vPos, 6, PINK);

      DrawLineEx(uPos, vPos, 2, RED);
      
      // Kenarın ağırlığı
      char label[5];
      DrawText( TextFormat("%d", weigth(g, u, v))
              , wPos.x - 5, wPos.y - 5, 20, WHITE); 
    }

    // Düğümler
    for (int i = 0; i < g.vertices; i++) {
      Vector2 pos = Vector2Subtract(positions[i], offset);

      DrawCircleV(pos, 30, BLUE);
      
      DrawText( TextFormat("%c", 'A'+i)
              , pos.x - 5, pos.y - 5, 20, WHITE);
    }


    EndDrawing();
  }

  return 0;
}
