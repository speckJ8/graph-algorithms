/**
 * Boruvka's Algorithm (computation of the Minimum Spanning Tree).
 * */
#ifndef __BORUVKA__
#define __BORUVKA__

#include <stdlib.h>
#include <stdbool.h>

/**
 * Data structure to keep track of a graph vertex.
 *
 * Besides keeping data about the vertex, this data structure is the element
 * of the linked list used to keep track of
 * the connected components during the execution of Boruvka's algorithm.
 * We use a linked list to manage the components so that to merge components,
 * represented by vertices `a` and `b`, we only need to set `a->next = b`
 * without having to make copies.
 */
struct BoruvkaVertex {
    // the label of the vertex
    int label;
    struct BoruvkaVertex** adjacent_vertices;
    size_t adjacent_vertices_count;
    // weights of the arrows incident to this vertex
    // `adjacent_vertex_weights[i]` is the weight of the edge
    // connecting this vertex to the vertex `adjacent_vertices[i]`
    int* adjacent_vertex_weights;
    // pointer to the head of the component list where thisvertex is
    // currently
    struct BoruvkaVertex* component_head;
    // next vertex in the component where this vertex is currently
    struct BoruvkaVertex* component_next;
    // indicates whether or not this vertex is the head of some
    // component
    bool is_head;
};

typedef struct BoruvkaVertex BoruvkaVertex;
typedef BoruvkaVertex** BoruvkaGraph;

/**
 * Compute the minimum spanning tree of a graph.
 *
 * @param graph the graph from which to compute the MST
 * @param size the number of vertices in the graph
 * */
void boruvka (BoruvkaGraph graph, size_t size);


#endif // __BORUVKA__
