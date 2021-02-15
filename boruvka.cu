/**
 *
 * Parellelized version of Boruvka's Algorithm
 * (computation of the Minimum Spanning Tree) using CUDA.
 *
 * */


#include <iostream>
#include <cmath>
#include <cassert>
#include <cuda_profiler_api.h>


/**
 * Error checking function adapted from:
 * https://developer.nvidia.com/blog/how-optimize-data-transfers-cuda-cc/
 */
#define check_cuda(ans) { _check((ans), __FILE__, __LINE__); }
inline cudaError_t _check (cudaError_t result, const char *file, int line) {
#if defined(DEBUG) || defined(_DEBUG)
    if (result != cudaSuccess) {
        fprintf(stderr, "CUDA Runtime Error: %s\n",
                cudaGetErrorString(result), __FILE__, __LINE__);
        assert(result == cudaSuccess);
    }
#endif
    return result;
}


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
    // list of the labels of adjacent vertices
    BoruvkaVertex** adjacent_vertices;
    // number of adjacent vertices
    size_t adjacent_vertices_count;
    // weights of the arrows incident to this vertex
    // `adjacent_vertex_weights[i]` is the weight of the edge
    // connecting this vertex to the vertex `adjacent_vertices[i]`
    int* adjacent_vertex_weights;
    // pointer to the head of the component list where thisvertex is
    // currently
    BoruvkaVertex* component_head;
    // next vertex in the component where this vertex is currently
    BoruvkaVertex* component_next;
    // indicates whether or not this vertex is the head of some
    // component
    bool is_head;
};

typedef BoruvkaVertex** BoruvkaGraph;


/**
 * Find the edge of minimal cost that is incident to a component.
 * 
 * @param graph                the adjacency list of the graph
 * @param components           the list of current components
 * @param component_count      the number of components
 *
 * @return the edge found for `component[c]` will have its source in
           `found_source[c]` and its target in `found_target[c]`
 */
__global__
void boruvka_find_min_edges (BoruvkaVertex** component_heads,
                             BoruvkaVertex** found_source,
                             BoruvkaVertex** found_target,
                             size_t component_count) {

    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int c = index; c < component_count; c += stride) {
        BoruvkaVertex* curr = component_heads[c];
        BoruvkaVertex* edge_source = NULL;
        BoruvkaVertex* edge_target = NULL;
        int edge_weight = -1;
        // find a vertex that is in a different component that has
        // the minimum weight possible
        while (curr != NULL) {
            for (int i = 0; i < curr->adjacent_vertices_count; i++) {
                BoruvkaVertex* adjacent_vertex = curr->adjacent_vertices[i];
                if (adjacent_vertex->component_head != curr->component_head &&
                    (curr->adjacent_vertex_weights[i] < edge_weight ||
                     edge_weight == -1)) {
                    edge_source = curr;
                    edge_target = adjacent_vertex;
                    edge_weight = curr->adjacent_vertex_weights[i];
                }
            }
            // next vertex in the component
            curr = curr->component_next;
        }
        if (edge_source != NULL && edge_target != NULL) {
            found_source[c] = edge_source;
            found_target[c] = edge_target;
        }
    }
}


size_t boruvka_merge_components (BoruvkaVertex** found_source,
                                 BoruvkaVertex** found_target,
                                 size_t component_count) {
    size_t _component_count = component_count;
    for (int c = 0; c < component_count; c++) {
        // source and target vertices of the edge found for
        // the current component
        BoruvkaVertex* source = found_source[c];
        BoruvkaVertex* target= found_target[c];

        if (target->component_head == source->component_head) {
            continue;
        }

        printf("-- new edge (%d, %d)\n", source->label, target->label);

        if (source != NULL && target != NULL) {
            BoruvkaVertex* curr = source;
            while (curr->component_next != NULL) {
                curr = curr->component_next;
            }
            // merge the component containing `source` with the component
            // containing `target` by making the last vertex in the former
            // component point to the first vertex in the latter
            curr->component_next = target->component_head;

            // now update the vertices in the component of `target` by
            // setting their head to be the head of the new merged component
            curr = target->component_head;
            target->component_head->is_head = false;
            while (curr != NULL) {
                curr->component_head = source->component_head;
                curr = curr->component_next;
            }
            _component_count -= 1;
        }
    }
    return _component_count;
}


void boruvka (BoruvkaGraph graph, size_t vertex_count) {

    size_t component_count = vertex_count;
    int threads = 1024;

    BoruvkaVertex** component_heads;
    BoruvkaVertex** found_sources;
    BoruvkaVertex** found_targets;
    cudaMallocManaged(&component_heads, component_count*sizeof(BoruvkaVertex*));
    cudaMallocManaged(&found_sources, component_count*sizeof(BoruvkaVertex*));
    cudaMallocManaged(&found_targets, component_count*sizeof(BoruvkaVertex*));

    // start with `vertex_count` components, each being a trivial
    // tree with one vertex and no edges
    for (int v = 0; v < vertex_count; v++) {
        component_heads[v] = graph[v];
        component_heads[v]->component_head = component_heads[v];
        component_heads[v]->component_next = NULL;
        component_heads[v]->is_head = true;
    }

    while (component_count != 1) {
        int blocks = (component_count + threads - 1) / threads;
        boruvka_find_min_edges<<<threads, blocks>>>(
                component_heads,
                found_sources,
                found_targets,
                component_count
        );
        check_cuda( cudaDeviceSynchronize() );

        size_t new_component_count = boruvka_merge_components(
                found_sources,
                found_targets,
                component_count
        );

        BoruvkaVertex** new_component_heads;
        cudaMallocManaged(
                &new_component_heads,
                new_component_count*sizeof(BoruvkaVertex*)
        );

        int nc = 0;
        for (int c = 0; c < component_count; c++) {
            if (component_heads[c]->is_head) {
                new_component_heads[nc++] = component_heads[c];
            }
        }

        cudaFree(component_heads);
        component_heads = new_component_heads;
        component_count = new_component_count;
    }

    cudaFree(component_heads);
    cudaFree(found_sources);
    cudaFree(found_targets);
}


int main (void) {
    size_t SIZE = 4;
    BoruvkaVertex *a, *b, *c, *d;
    cudaMallocManaged(&a, sizeof(BoruvkaVertex));
    cudaMallocManaged(&b, sizeof(BoruvkaVertex));
    cudaMallocManaged(&c, sizeof(BoruvkaVertex));
    cudaMallocManaged(&d, sizeof(BoruvkaVertex));

    /**
     * Initialize the simple graph:
     *        1
     *    0 ----- 1
     *    |       |
     *  3 |       | 2
     *    |       |
     *    3 ----- 2
     *        1
     * */

    a->label = 0;
    a->adjacent_vertices_count = 2;
    cudaMallocManaged(
            &a->adjacent_vertices,
            a->adjacent_vertices_count*sizeof(BoruvkaVertex*)
    );
    cudaMallocManaged(
            &a->adjacent_vertex_weights,
            a->adjacent_vertices_count*sizeof(BoruvkaVertex*)
    );
    a->adjacent_vertices[0] = b;
    a->adjacent_vertex_weights[0] = 1;
    a->adjacent_vertices[1] = d;
    a->adjacent_vertex_weights[1] = 3;

    b->label = 1;
    b->adjacent_vertices_count = 2;
    cudaMallocManaged(
            &b->adjacent_vertices,
            b->adjacent_vertices_count*sizeof(BoruvkaVertex*)
    );
    cudaMallocManaged(
            &b->adjacent_vertex_weights,
            b->adjacent_vertices_count*sizeof(BoruvkaVertex*)
    );
    b->adjacent_vertices[0] = a;
    b->adjacent_vertex_weights[0] = 1;
    b->adjacent_vertices[1] = c;
    b->adjacent_vertex_weights[1] = 2;

    c->label = 2;
    c->adjacent_vertices_count = 2;
    cudaMallocManaged(
            &c->adjacent_vertices,
            c->adjacent_vertices_count*sizeof(BoruvkaVertex*)
    );
    cudaMallocManaged(
            &c->adjacent_vertex_weights,
            c->adjacent_vertices_count*sizeof(BoruvkaVertex*)
    );
    c->adjacent_vertices[0] = b;
    c->adjacent_vertex_weights[0] = 2;
    c->adjacent_vertices[1] = d;
    c->adjacent_vertex_weights[1] = 1;

    d->label = 3;
    d->adjacent_vertices_count = 2;
    cudaMallocManaged(
            &d->adjacent_vertices,
            d->adjacent_vertices_count*sizeof(BoruvkaVertex*)
    );
    cudaMallocManaged(
            &d->adjacent_vertex_weights,
            d->adjacent_vertices_count*sizeof(BoruvkaVertex*)
    );
    d->adjacent_vertices[0] = c;
    d->adjacent_vertex_weights[0] = 1;
    d->adjacent_vertices[1] = a;
    d->adjacent_vertex_weights[1] = 3;

    BoruvkaGraph graph;
    cudaMallocManaged(&graph, SIZE*sizeof(BoruvkaVertex*));
    graph[0] = a;
    graph[1] = b;
    graph[2] = c;
    graph[3] = d;

    boruvka(graph, SIZE);

    cudaFree(a->adjacent_vertices);
    cudaFree(a->adjacent_vertex_weights);
    cudaFree(a);

    cudaFree(b->adjacent_vertices);
    cudaFree(b->adjacent_vertex_weights);
    cudaFree(b);

    cudaFree(c->adjacent_vertices);
    cudaFree(c->adjacent_vertex_weights);
    cudaFree(c);

    cudaFree(d->adjacent_vertices);
    cudaFree(d->adjacent_vertex_weights);
    cudaFree(d);

    cudaFree(graph);

    return 0;
}
