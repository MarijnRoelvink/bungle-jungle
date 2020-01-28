//The graph with neighbours
const Edge graph[] = 
{{10, 6},
{6, 18},
{6, 17},
{6,2},
{18, 5},
{17, 5},
{18, 1},
{17, 1},
{17, 2},
{18, 2},
{6, 11},
{11, 7},
{11, 15},
{7, 15},
{9, 13},
{5, 13},
{5, 14},
{5, 1},
{7, 3},
{3, 14},
{3, 15},
{1, 14},
{7, 16},
{7, 4},
{16, 12},
{12, 8},
{8, 4},
{8, 16},
{16, 4},
{4, 19},
{4, 20},
{8, 20}};

//goal: change to the correct function
//input: edge = edge connected to neighbour v, v = id to find neighbour of.
//output: the neighbour of int v at the other side of the edge
int getNeighbour(Edge edge, int v) {
  if(edge.a == v) {
    return edge.b;
  } else if(edge.b == v) {
    return edge.a;
  } else {
    return -1;
  }
}

//goal: get the amount of neighbours from id
//input: id = id of step to find amount of neighbours of
//output: the size of neighbours
int getNeighboursSize(int id) {
  int graphSize = sizeof(graph)/sizeof(Edge);
  int totalSize = 0;
  for(int i = 0; i < graphSize; i++) {
    int neighbour = getNeighbour(graph[i], id);
    if(neighbour != -1) {
      totalSize++;
    }
  }
  return totalSize;
}

//goal: get the neighbours from id
//input: neighbs = pointer to neighbours, id = id of step
//output: pointer to neighbours
int* getNeighbours(int* neighbs, int id) {
  int graphSize = sizeof(graph)/sizeof(Edge);
  int totalSize = 0;
  for(int i = 0; i < graphSize; i++) {
    int neighbour = getNeighbour(graph[i], id);
    if(neighbour != -1) {
      neighbs[totalSize] = neighbour;
      totalSize++;
    }
  }
  return neighbs;
}

//goal: get the poleneighbours from id
//input: poleNeighbs = pointer to poleneighbours, id = id of step
//output: pointer to poleneighbours
int* getPoleNeighbours(int* poleNeighbs, int id) {
  id--;
  int polePos = id%4;
  int poleBase = id - polePos;
  int index = 0;
  for(int i = 0; i < 4; i++) {
    if(i != polePos) {
      poleNeighbs[index] = poleBase + i + 1;
      index++;
    }
  }
  return poleNeighbs;
}
