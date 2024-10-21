#include <string.h>
#include <cstdio>
#include <cstdlib>

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

unsigned particle_neighborhood(unsigned* buckets, particle_t* p, float h)
{
	// Given a particle p, find neighboring buckets
    /* BEGIN TASK */
    float ix = p->x[0]; // X coordinate bin index
    float iy = p->x[1]; // Y coordinate bin index
    float iz = p->x[2]; // Z coordinate bin index

    int num_bins = 0;
    int num_not_bins = 0;

    // this might not work for edge cases:
    // Iterate over a 3x3x3 neighborhood of bins in 3D (or 3x3 in 2D)
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dz = -1; dz <= 1; ++dz)
            {
                float nx = (ix + h * dx);
                float ny = (iy + h * dy);
                float nz = (iz + h * dz);

                if(nx >= 0. && nx <= 1. && ny >= 0. && ny <= 1. && nz >= 0. && nz <= 1.) {
                    // get bin index with zmorton
                    buckets[num_bins++] = zm_encode(((unsigned) (nx / h)) & HASH_MASK, ((unsigned) (ny / h)) & HASH_MASK, ((unsigned) (nz / h)) & HASH_MASK);
                }
                // else{
                //     printf("{%f, %f, %f}\n", nx, ny, nz);
                //     num_not_bins++;
                // }
            }
        }
    }
    // if(num_bins == 27){
    //     printf("%d is num_bins\n", num_bins);
    //     exit(0);
    // }

    // printf("%d  num_not_bins\n", num_not_bins);
    // exit(0);

    return num_bins; // number of bins populated
    /* END TASK */
}

void hash_particles(sim_state_t* s, float h)
{
    int n = s->n;
    // particle_t* p = s->part;
    // particle_t** hash = s->hash;

    // Clear the hash table
    // #pragma omp parallel for
    // for (int i = 0; i < HASH_SIZE; ++i) {
    //     hash[i] = NULL;
    // }

    // // Assign particles to hash buckets
    // // #pragma omp parallel for
    // for (int i = 0; i < n; ++i) {
    //     unsigned bucket = particle_bucket(&p[i], h);

    //     // Insert particle into the bucket's linked list
    //     // Use atomic operations to prevent race conditions
    //     // #pragma omp critical
    //     {
    //         p[i].next = hash[bucket];
    //         hash[bucket] = &p[i];
    //     }
    // }
	particle_t* curr;
	unsigned zm_hash;

    // #pragma omp parallel for
    for (int i = 0; i < HASH_SIZE; ++i) {
        s->hash[i] = 0;
    }

	// TO DO:
	// Iterate through each particle
	// Determine bin of particle (e.g. closest bin midpoint)
	// Figure out mapping of particle coordinates to integer input to zmorton
	// Use zmorton as hash for a bucket
	

    // #pragma omp parallel for
	for(int i = 0; i < s->n; ++i) {
		curr = &s->part[i];
		zm_hash = particle_bucket(curr, h);
        // printf("ZM_HASH: %8x\n", zm_hash);
        // #pragma omp critical
        // {
        //     curr->next =s->hash[zm_hash];
        //     s->hash[zm_hash] = curr;
        // }
        // #pragma omp critical 
        {
            curr->next = s->hash[zm_hash];
            s->hash[zm_hash] = curr;
        }
	}
}
