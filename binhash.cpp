#include <string.h>
#include <cstdio>

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
    unsigned zm_index = particle_bucket(p, h);


    int ix = (int) p->x[0] / h; // X coordinate bin index
    int iy =(int) p->x[1] / h; // Y coordinate bin index
    int iz = (int) p->x[2] / h; // Z coordinate bin index

    int num_bins = 0;

    // this might not work for edge cases:
    // Iterate over a 3x3x3 neighborhood of bins in 3D (or 3x3 in 2D)
    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            for (int dz = -1; dz <= 1; ++dz)
            {
                int nx = (ix + dx) & HASH_MASK;
                int ny = (iy + dy) & HASH_MASK;
                int nz = (iz + dz) & HASH_MASK;

                if(nx >= 0 && nx <= 1 && ny >= 0 && ny <= 1 && nz >= 0 && nz <= 1) {
                    // get bin index with zmorton
                    buckets[num_bins++] = zm_encode(nx, ny, nz);
                }
            }
        }
    }
    return num_bins; // number of bins populated
    /* END TASK */
}

void hash_particles(sim_state_t* s, float h)
{
	particle_t* curr;
	unsigned zm_hash;
	// TO DO:
	// Iterate through each particle
	// Determine bin of particle (e.g. closest bin midpoint)
	// Figure out mapping of particle coordinates to integer input to zmorton
	// Use zmorton as hash for a bucket
	
	for(int i = 0; i < s->n; ++i) {
		curr = &s->part[i];
		zm_hash = particle_bucket(curr, h);
        // printf("ZM_HASH: %8x\n", zm_hash);
		if(s->hash[zm_hash] == 0){
			s->hash[zm_hash] = curr;
            s->hash[zm_hash] = nullptr;
		}
		else{
			curr->next = s->hash[zm_hash];
			s->hash[zm_hash] = curr;
		}
	}
}
