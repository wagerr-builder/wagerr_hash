#include "wagerr.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "sha3/sph_blake.h"
#include "sha3/sph_bmw.h"
#include "sha3/sph_groestl.h"
#include "sha3/sph_jh.h"
#include "sha3/sph_keccak.h"
#include "sha3/sph_skein.h"
#include "sha3/sph_luffa.h"
#include "sha3/sph_cubehash.h"
#include "sha3/sph_shavite.h"
#include "sha3/sph_simd.h"
#include "sha3/sph_echo.h"


void wagerr_hash(const char* input, int len, char* output)
{
    sph_blake512_context     ctx_blake;
    sph_bmw512_context       ctx_bmw;
    sph_groestl512_context   ctx_groestl;
    sph_skein512_context     ctx_skein;
    sph_jh512_context        ctx_jh;
    sph_keccak512_context    ctx_keccak;

    //these uint512 in the c++ source of the client are backed by an array of uint32
    uint32_t hashA[16], hashB[16];
    uint32_t mask = 8;
    uint32_t zero = 0;

    sph_blake512_init(&ctx_blake);
    sph_blake512 (&ctx_blake, input, len);
    sph_blake512_close (&ctx_blake, hashA);

    sph_bmw512_init(&ctx_bmw);
    sph_bmw512 (&ctx_bmw, hashA, 64);
    sph_bmw512_close(&ctx_bmw, hashB);

    if ((hashB[0] & mask) != zero)
    {
      sph_groestl512_init(&ctx_groestl);
      sph_groestl512(&ctx_groestl, hashB, 64); // 1
      sph_groestl512_close(&ctx_groestl, hashA); // 2
    } else {
      sph_skein512_init(&ctx_skein);
      sph_skein512(&ctx_skein, hashB, 64); // 1
      sph_skein512_close(&ctx_skein, hashA); // 2
    }

    sph_groestl512_init(&ctx_groestl);
    sph_groestl512 (&ctx_groestl, hashA, 64);
    sph_groestl512_close(&ctx_groestl, hashB);

    sph_jh512_init(&ctx_jh);
    sph_jh512 (&ctx_jh, hashB, 64);
    sph_jh512_close(&ctx_jh, hashA);

    if ((hashA[0] & mask) != zero) // 4
    {
      sph_blake512_init(&ctx_blake);
      sph_blake512(&ctx_blake, hashA, 64); //
      sph_blake512_close(&ctx_blake, hashB); // 5
    } else {
      sph_bmw512_init(&ctx_bmw);
      sph_bmw512(&ctx_bmw, hashA, 64); // 4
      sph_bmw512_close(&ctx_bmw, hashB); // 5
    }

    sph_keccak512_init(&ctx_keccak);
    sph_keccak512 (&ctx_keccak, hashB, 64);
    sph_keccak512_close(&ctx_keccak, hashA);

    sph_skein512_init(&ctx_skein);
    sph_skein512(&ctx_skein, hashA, 64); // 6
    sph_skein512_close(&ctx_skein, hashB); // 7

    if ((hashB[0] & mask) != zero) // 7
    {
      sph_keccak512_init(&ctx_keccak);
      sph_keccak512(&ctx_keccak, hashB, 64); //
      sph_keccak512_close(&ctx_keccak, hashA); // 8
    } else {
      sph_jh512_init(&ctx_jh);
      sph_jh512(&ctx_jh, hashB, 64); // 7
      sph_jh512_close(&ctx_jh, hashA); // 8
    }

    memcpy(output, hashA, 32);
}

