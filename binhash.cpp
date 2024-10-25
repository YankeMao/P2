#include <string.h>
#include <math.h> 

#include "zmorton.hpp"
#include "binhash.hpp"

/*@q
 * ====================================================================
 */

/*@T
 * \subsection{Spatial hashing implementation}
 * 
 * In the current implementation, we assume [[HASH_DIM]] is $2^b$,
 * so that computing a bitwise of an integer with [[HASH_DIM]] extracts
 * the $b$ lowest-order bits.  We could make [[HASH_DIM]] be something
 * other than a power of two, but we would then need to compute an integer
 * modulus or something of that sort.
 * 
 *@c*/

#define HASH_MASK (HASH_DIM-1)

unsigned particle_bucket(particle_t* p, float h)
{
    unsigned ix = p->x[0]/h;
    unsigned iy = p->x[1]/h;
    unsigned iz = p->x[2]/h;
    return zm_encode(ix & HASH_MASK, iy & HASH_MASK, iz & HASH_MASK);
}

unsigned particle_neighborhood(particle_t** buckets, particle_t* p, float h)
{
    // Calculate the cell coordinates of the particle
    int ix = floor(p->x[0] / h);
    int iy = floor(p->x[1] / h);
    int iz = floor(p->x[2] / h);

    // unsigned hash_index = zm_encode(ix & HASH_MASK, iy & HASH_MASK, iz & HASH_MASK);

    // Array to hold offsets for neighboring cells (including the current cell)
    int offsets[27][3];
    int idx = 0;
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dz = -1; dz <= 1; ++dz) {
                offsets[idx][0] = ix + dx;
                offsets[idx][1] = iy + dy;
                offsets[idx][2] = iz + dz;
                ++idx;
            }
        }
    }

    // Iterate through all neighboring cells and collect particles
    unsigned neighborhood_size = 0;
    for (int i = 0; i < 27; ++i) {
        unsigned neighbor_hash = zm_encode(
            offsets[i][0] & HASH_MASK,
            offsets[i][1] & HASH_MASK,
            offsets[i][2] & HASH_MASK
        );

        particle_t* current = buckets[neighbor_hash];
        while (current != NULL) {
            // Optional: Add filtering based on actual distance here
            // For now, we just count the number of potential neighbors
            neighborhood_size++;
            current = current->next;
        }
    }

    return neighborhood_size;
}

void hash_particles(sim_state_t* s, float h)
{
    int n = s->n;
    particle_t* p = s->part;
    particle_t** hash = s->hash;

    // Clear the hash table
    for (int i = 0; i < HASH_SIZE; ++i) {
        hash[i] = NULL;
    }

    // Assign particles to hash buckets
    for (int i = 0; i < n; ++i) {
        unsigned bucket = particle_bucket(&p[i], h);

        // Insert particle into the bucket's linked list
        // Use atomic operations to prevent race conditions
        p[i].next = hash[bucket];
        hash[bucket] = &p[i];
    }
}
