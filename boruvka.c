/**
 *
 * Boruvka's Algorithm (computation of the Minimum Spanning Tree).
 *
 * */

#include <stdio.h>
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
    // list of the labels of adjacent vertices
    struct BoruvkaVertex** adjacent_vertices;
    // number of adjacent vertices
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
 * Find the edge of minimal cost that is incident to a component.
 * 
 * @param graph                the adjacency list of the graph
 * @param components           the list of current components
 * @param component_count      the number of components
 *
 * @return the edge found for `component[c]` will have its source in
           `found_source[c]` and its target in `found_target[c]`
 */
void boruvka_find_min_edges (BoruvkaVertex** component_heads,
                             BoruvkaVertex** found_source,
                             BoruvkaVertex** found_target,
                             size_t component_count) {

    for (int c = 0; c < component_count; c++) {
        BoruvkaVertex* curr = component_heads[c];
        BoruvkaVertex* edge_source = NULL;
        BoruvkaVertex* edge_target = NULL;
        int edge_weight = -1;
        // find a vertex that is in a different component that has
        // the minimum weight possible
        while (curr != NULL) {
            // go through the vertices that are adjacent to `curr` and
            // that are in different components
            // if the edge connecting them is minimum then we choose that edge
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

    BoruvkaVertex** component_heads = malloc(
            component_count*sizeof(BoruvkaVertex*)
    );

    // start with `vertex_count` components, each being a trivial
    // tree with one vertex and no edges
    for (int v = 0; v < vertex_count; v++) {
        component_heads[v] = graph[v];
        component_heads[v]->component_head = component_heads[v];
        component_heads[v]->component_next = NULL;
        component_heads[v]->is_head = true;
    }

    BoruvkaVertex** found_sources = malloc(
            component_count*sizeof(BoruvkaVertex*)
    );
    BoruvkaVertex** found_targets = malloc(
            component_count*sizeof(BoruvkaVertex*)
    );

    while (component_count != 1) {
        boruvka_find_min_edges(
                component_heads,
                found_sources,
                found_targets,
                component_count
        );
        size_t new_component_count = boruvka_merge_components(
                found_sources,
                found_targets,
                component_count
        );

        BoruvkaVertex** new_component_heads = malloc(
                new_component_count*sizeof(BoruvkaVertex*)
        );

        found_sources = realloc(
                found_sources,
                new_component_count*sizeof(BoruvkaVertex*)
        );
        found_targets = realloc(
                found_targets,
                new_component_count*sizeof(BoruvkaVertex*)
        );

        int nc = 0;
        for (int c = 0; c < component_count; c++) {
            if (component_heads[c]->is_head) {
                new_component_heads[nc++] = component_heads[c];
            }
        }

        free(component_heads);
        component_heads = new_component_heads;
        component_count = new_component_count;
    }

    free(component_heads);
    free(found_sources);
    free(found_targets);
}

int main (int argc, char** argv) {
    size_t SIZE = 4;
    BoruvkaVertex* a = malloc(sizeof(BoruvkaVertex));
    BoruvkaVertex* b = malloc(sizeof(BoruvkaVertex));
    BoruvkaVertex* c = malloc(sizeof(BoruvkaVertex));
    BoruvkaVertex* d = malloc(sizeof(BoruvkaVertex));

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
    a->adjacent_vertices =
        malloc(a->adjacent_vertices_count*sizeof(BoruvkaVertex*));
    a->adjacent_vertex_weights =
        malloc(a->adjacent_vertices_count*sizeof(BoruvkaVertex*));
    a->adjacent_vertices[0] = b;
    a->adjacent_vertex_weights[0] = 1;
    a->adjacent_vertices[1] = d;
    a->adjacent_vertex_weights[1] = 3;

    b->label = 1;
    b->adjacent_vertices_count = 2;
    b->adjacent_vertices =
        malloc(b->adjacent_vertices_count*sizeof(BoruvkaVertex*));
    b->adjacent_vertex_weights =
        malloc(b->adjacent_vertices_count*sizeof(BoruvkaVertex*));
    b->adjacent_vertices[0] = a;
    b->adjacent_vertex_weights[0] = 1;
    b->adjacent_vertices[1] = c;
    b->adjacent_vertex_weights[1] = 2;

    c->label = 2;
    c->adjacent_vertices_count = 2;
    c->adjacent_vertices =
        malloc(c->adjacent_vertices_count*sizeof(BoruvkaVertex*));
    c->adjacent_vertex_weights =
        malloc(d->adjacent_vertices_count*sizeof(BoruvkaVertex*));
    c->adjacent_vertices[0] = b;
    c->adjacent_vertex_weights[0] = 2;
    c->adjacent_vertices[1] = d;
    c->adjacent_vertex_weights[1] = 1;

    d->label = 3;
    d->adjacent_vertices_count = 2;
    d->adjacent_vertices =
        malloc(d->adjacent_vertices_count*sizeof(BoruvkaVertex*));
    d->adjacent_vertex_weights =
        malloc(d->adjacent_vertices_count*sizeof(BoruvkaVertex*));
    d->adjacent_vertices[0] = c;
    d->adjacent_vertex_weights[0] = 1;
    d->adjacent_vertices[1] = a;
    d->adjacent_vertex_weights[1] = 3;

    BoruvkaGraph graph = malloc(SIZE*sizeof(BoruvkaVertex*));
    graph[0] = a;
    graph[1] = b;
    graph[2] = c;
    graph[3] = d;

    boruvka(graph, SIZE);

    free(a->adjacent_vertices);
    free(a->adjacent_vertex_weights);
    free(a);

    free(b->adjacent_vertices);
    free(b->adjacent_vertex_weights);
    free(b);

    free(c->adjacent_vertices);
    free(c->adjacent_vertex_weights);
    free(c);

    free(d->adjacent_vertices);
    free(d->adjacent_vertex_weights);
    free(d);

    free(graph);

    return 0;
}
