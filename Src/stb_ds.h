// stb_ds.h - v0.1 - public domain data structures - http://nothings.org/stb_ds

#ifndef INCLUDE_STB_DS_H
#define INCLUDE_STB_DS_H

#include "shared.h"

// for security against attackers, seed the library with a random number, at least time() but stronger is better
extern void stbds_rand_seed(size_t seed);

// these are the hash functions used internally if you want to test them or use them for other purposes
extern size_t stbds_hash_bytes(void *p, size_t len, size_t seed);

// have to #define STBDS_UNIT_TESTS to call this
extern void stbds_unit_tests(void);

///////////////
//
// Everything below here is implementation details
//

extern void * stbds_arrgrowf(void *a, size_t elemsize, size_t addlen, size_t min_cap);
extern void   stbds_hmfree_func(void *p, size_t elemsize, size_t keyoff);
extern void * stbds_hmget_key(void *a, size_t elemsize, void *key, size_t keysize, int mode);
extern void * stbds_hmput_default(void *a, size_t elemsize);
extern void * stbds_hmput_key(void *a, size_t elemsize, void *key, size_t keysize, int mode);
extern void * stbds_hmdel_key(void *a, size_t elemsize, void *key, size_t keysize, size_t keyoffset, int mode);

#if defined(__GNUC__) || defined(__clang__)
#define STBDS_HAS_TYPEOF
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#define STBDS_HAS_LITERAL_ARRAY
#endif

// this macro takes the address of the argument, but on gcc/clang can accept rvalues
#if defined(STBDS_HAS_LITERAL_ARRAY) && defined(STBDS_HAS_TYPEOF)
#define STBDS_ADDRESSOF(typevar, value)     ((typeof(typevar)[1]){value}) // literal array decays to pointer to value
#else
#define STBDS_ADDRESSOF(typevar, value)     &(value)
#endif

#define STBDS_OFFSETOF(var,field)           ((char *) &(var)->field - (char *) (var))

#define stbds_header(t)  ((stbds_array_header *) (t) - 1)
#define stbds_temp(t)    stbds_header(t)->temp

#define stbds_arrsetcap(a,n) (stbds_arrgrow(a,0,n))
#define stbds_arrsetlen(a,n) ((stbds_arrcap(a) < n ? stbds_arrsetcap(a,n),0 : 0), (a) ? stbds_header(a)->length = (n) : 0)
#define stbds_arrcap(a)       ((a) ? stbds_header(a)->capacity : 0)
#define stbds_arrlen(a)       ((a) ? (ptrdiff_t) stbds_header(a)->length : 0)
#define stbds_arrlenu(a)      ((a) ?             stbds_header(a)->length : 0)
#define stbds_arrput(a,v)     (stbds_arrmaybegrow(a,1), (a)[stbds_header(a)->length++] = (v))
#define stbds_arrpush         stbds_arrput  // synonym
#define stbds_arraddn(a,n)    (stbds_arrmaybegrow(a,n), stbds_header(a)->length += (n))
#define stbds_arrlast(a)      ((a)[stbds_header(a)->length-1])
#define stbds_arrfree(a)      ((void) ((a) ? realloc(stbds_header(a),0) : 0), (a)=NULL)
#define stbds_arrdel(a,i)     stbds_arrdeln(a,i,1)
#define stbds_arrdeln(a,i,n)  (memmove(&(a)[i], &(a)[(i)+(n)], sizeof *(a) * (stbds_header(a)->length-(n)-(i))), stbds_header(a)->length -= (n))
#define stbds_arrdelswap(a,i) ((a)[i] = stbds_arrlast(a), stbds_header(a)->length -= 1)
#define stbds_arrinsn(a,i,n)  (stbds_arraddn((a),(n)), memmove(&(a)[(i)+(n)], &(a)[i], sizeof *(a) * (stbds_header(a)->length-(n)-(i))))
#define stbds_arrins(a,i,v)   (stbds_arrinsn((a),(i),1), (a)[i]=(v))

#define stbds_arrmaybegrow(a,n)  ((!(a) || stbds_header(a)->length + (n) > stbds_header(a)->capacity) \
                                  ? (stbds_arrgrow(a,n,0),0) : 0)

#define stbds_arrgrow(a,b,c)   ((a) = stbds_arrgrowf((a), sizeof *(a), (b), (c)))

#define stbds_hmput(t, k, v) \
    ((t) = stbds_hmput_key_wrapper((t), sizeof *(t), STBDS_ADDRESSOF((t)->key, (k)), sizeof (t)->key, 0),   \
     (t)[stbds_temp((t)-1)].key = (k), \
     (t)[stbds_temp((t)-1)].value = (v))

#define stbds_hmputs(t, s) \
    ((t) = stbds_hmput_key_wrapper((t), sizeof *(t), &(s).key, sizeof (s).key, STBDS_HM_BINARY), \
     (t)[stbds_temp((t)-1)] = (s))

#define stbds_hmgeti(t,k) \
    ((t) = stbds_hmget_key_wrapper((t), sizeof *(t), STBDS_ADDRESSOF((t)->key, (k)), sizeof (t)->key, STBDS_HM_BINARY), \
      stbds_temp((t)-1))

#define stbds_hmgetp(t, k) \
    ((void) stbds_hmgeti(t,k), &(t)[stbds_temp((t)-1)])

#define stbds_hmdel(t,k) \
    (stbds_hmdel_key((t),sizeof *(t), STBDS_ADDRESSOF((t)->key, (k)), sizeof (t)->key, STBDS_OFFSETOF((t),key), STBDS_HM_BINARY))

#define stbds_hmdefault(t, v) \
    ((t) = stbds_hmput_default_wrapper((t), sizeof *(t)), (t)[-1].value = (v))

#define stbds_hmdefaults(t, s) \
    ((t) = stbds_hmput_default_wrapper((t), sizeof *(t)), (t)[-1] = (s))

#define stbds_hmfree(p)        \
    ((void) ((p) != NULL ? stbds_hmfree_func((p)-1,sizeof*(p),STBDS_OFFSETOF((p),key)),0 : 0),(p)=NULL)

#define stbds_hmgets(t, k) (*stbds_hmgetp(t,k))
#define stbds_hmget(t, k)  (stbds_hmgetp(t,k)->value)
#define stbds_hmlen(t)     (stbds_arrlen((t)-1)-1)
#define stbds_hmlenu(t)    (stbds_arrlenu((t)-1)-1)


typedef struct
{
  size_t      length;
  size_t      capacity;
  void      * hash_table;
  ptrdiff_t   temp;
} stbds_array_header;

enum
{
   STBDS_HM_BINARY,
};

#define stbds_arrgrowf_wrapper            stbds_arrgrowf
#define stbds_hmget_key_wrapper           stbds_hmget_key
#define stbds_hmput_default_wrapper       stbds_hmput_default
#define stbds_hmput_key_wrapper           stbds_hmput_key
#define stbds_hmdel_key_wrapper           stbds_hmdel_key

#endif // INCLUDE_STB_DS_H


//////////////////////////////////////////////////////////////////////////////
//
//   IMPLEMENTATION
//

#include <assert.h>

#ifndef STBDS_ASSERT
#define STBDS_ASSERT(x)   ((void) 0)
#endif


//
// stbds_arr implementation
//

void *stbds_arrgrowf(void *a, size_t elemsize, size_t addlen, size_t min_cap)
{
  void *b;
  size_t min_len = stbds_arrlen(a) + addlen;

  // compute the minimum capacity needed
  if (min_len > min_cap)
    min_cap = min_len;

  if (min_cap <= stbds_arrcap(a))
    return a;

  // increase needed capacity to guarantee O(1) amortized
  if (min_cap < 2 * stbds_arrcap(a))
    min_cap = 2 * stbds_arrcap(a);
  else if (min_cap < 4)
    min_cap = 4;

  b = realloc((a) ? stbds_header(a) : 0, elemsize * min_cap + sizeof(stbds_array_header));
  b = (char *) b + sizeof(stbds_array_header);
  if (a == NULL) {
    stbds_header(b)->length = 0;
    stbds_header(b)->hash_table = 0;
  }
  stbds_header(b)->capacity = min_cap;
  return b;
}

//
// stbds_hm hash table implementation
//

#define STBDS_CACHE_LINE_SIZE   64
#define STBDS_BUCKET_LENGTH      8
#define STBDS_BUCKET_SHIFT       3
#define STBDS_BUCKET_MASK       (STBDS_BUCKET_LENGTH-1)

#define STBDS_ALIGN_FWD(n,a)   (((n) + (a) - 1) & ~((a)-1))

typedef struct
{
   size_t    hash [STBDS_BUCKET_LENGTH];
   ptrdiff_t index[STBDS_BUCKET_LENGTH];
} stbds_hash_bucket; // in 32-bit, this is one 64-byte cache line; in 64-bit, each array is one 64-byte cache line

typedef struct
{
  size_t slot_count;
  size_t used_count;
  size_t used_count_threshold;
  size_t used_count_shrink_threshold;
  size_t tombstone_count;
  size_t tombstone_count_threshold;
  size_t seed;
  stbds_hash_bucket *storage; // not a separate allocation, just 64-byte aligned storage after this struct
} stbds_hash_index;

#define STBDS_INDEX_EMPTY    -1
#define STBDS_INDEX_DELETED  -2
#define STBDS_INDEX_IN_USE(x)  ((x) >= 0)

#define STBDS_HASH_EMPTY      0
#define STBDS_HASH_DELETED    1

static size_t stbds_hash_seed=0x31415926;

void stbds_rand_seed(size_t seed)
{
  stbds_hash_seed = seed;
}

#define stbds_load_32_or_64(var, temp, v32, v64_hi, v64_lo)                                          \
  temp = v64_lo ^ v32, temp <<= 16, temp <<= 16, temp >>= 16, temp >>= 16, /* discard if 32-bit */   \
  var = v64_hi, var <<= 16, var <<= 16,                                    /* discard if 32-bit */   \
  var ^= temp ^ v32

static stbds_hash_index *stbds_make_hash_index(size_t slot_count, stbds_hash_index *ot)
{
  stbds_hash_index *t;
  t = (stbds_hash_index *) realloc(0,(slot_count >> STBDS_BUCKET_SHIFT) * sizeof(stbds_hash_bucket) + sizeof(stbds_hash_index) + STBDS_CACHE_LINE_SIZE-1);
  t->storage = (stbds_hash_bucket *) STBDS_ALIGN_FWD((size_t) (t+1), STBDS_CACHE_LINE_SIZE);
  t->slot_count = slot_count;
  STBDS_ASSERT(t->slot_count != 50001);
  t->tombstone_count = 0;
  t->used_count = 0;
  t->used_count_threshold        = slot_count*13/16; // if 12/16th of table is occupied, rebuild
  t->tombstone_count_threshold   = slot_count* 1/16; // if 14/16th of table is occupied+tombstones, rebuild
  t->used_count_shrink_threshold = slot_count* 6/16; // if table is only 6/16th
  if (ot) {
    // reuse old seed so we can reuse old hashes so below "copy out old data" doesn't do any hashing
    t->seed = ot->seed;
  } else {
    size_t a,b,temp;
    t->seed = stbds_hash_seed;
    // LCG
    // in 32-bit, a =          2147001325   b =  715136305
    // in 64-bit, a = 2862933555777941757   b = 3037000493
    stbds_load_32_or_64(a,temp, 2147001325, 0x27bb2ee6, 0x87b0b0fd);
    stbds_load_32_or_64(b,temp,  715136305,          0, 0xb504f32d);
    stbds_hash_seed = stbds_hash_seed  * a + b;
  }

  {
    size_t i,j;
    for (i=0; i < slot_count >> STBDS_BUCKET_SHIFT; ++i) {
      stbds_hash_bucket *b = &t->storage[i];
      for (j=0; j < STBDS_BUCKET_LENGTH; ++j)
        b->hash[j] = STBDS_HASH_EMPTY;
      for (j=0; j < STBDS_BUCKET_LENGTH; ++j)
        b->index[j] = STBDS_INDEX_EMPTY;
    }
  }

  // copy out the old data, if any
  if (ot) {
    size_t i,j;
    t->used_count = ot->used_count;
    for (i=0; i < ot->slot_count >> STBDS_BUCKET_SHIFT; ++i) {
      stbds_hash_bucket *ob = &ot->storage[i];
      for (j=0; j < STBDS_BUCKET_LENGTH; ++j) {
        if (STBDS_INDEX_IN_USE(ob->index[j])) {
          size_t hash = ob->hash[j];
          size_t pos = hash & (t->slot_count-1);
          size_t step = STBDS_BUCKET_LENGTH;
          for (;;) {
            size_t limit,z;
            stbds_hash_bucket *bucket;
            pos &= (t->slot_count-1);
            bucket = &t->storage[pos >> STBDS_BUCKET_SHIFT];

            for (z=pos & STBDS_BUCKET_MASK; z < STBDS_BUCKET_LENGTH; ++z) {
              if (bucket->hash[z] == 0) {
                bucket->hash[z] = hash;
                bucket->index[z] = ob->index[j];
                goto done;
              }
            }

            limit = pos & STBDS_BUCKET_MASK;
            for (z = 0; z < limit; ++z) {
              if (bucket->hash[z] == 0) {
                bucket->hash[z] = hash;
                bucket->index[z] = ob->index[j];
                goto done;
              }
            }

            pos += step;                  // quadratic probing
            step += STBDS_BUCKET_LENGTH;
          }
        }
       done:
        ;
      }
    }
  }

  return t;
}

#if MCL_BITNESS == 64
#define FNV_PRIME 1099511628211u
#define FNV_OFFSET_BASIS 14695981039346656037u
#else
#define FNV_PRIME 16777619u
#define FNV_OFFSET_BASIS 2166136261u
#endif

size_t fnv1a(void* data, size_t len, size_t seed) {
  size_t hash = FNV_OFFSET_BASIS;
  hash ^= seed;
  for (size_t i = 0; i < len; i++) {
    hash *= FNV_PRIME;
    hash ^= ((char*)data)[i];
  }
  return hash;
}

size_t stbds_hash_bytes(void* p, size_t len, size_t seed)
{
  PackedVariant* vt = *(PackedVariant**)p;
  int64_t hash = fnv1a(&vt->type, sizeof(vt->type) + vt->len, seed);
  DebugStrPtr(L"hash", hash);
  return hash;
}

static int stbds_is_key_equal(void* a, size_t elemsize, void* key, size_t keysize, int mode, size_t i)
{
  PackedVariant* pvA = *(PackedVariant**)((char*)a + elemsize * i);
  PackedVariant* pvKey = *(PackedVariant**)key;

  DebugStrPtr(L"Size", elemsize);

  DebugStrPtr(L"Comparing against index", i);

  // guard same width
  if (pvA->len != pvKey->len) {
    DebugStr(L"Not equal different lengths!");
    DebugPtr(pvA);
    DebugPtr(pvKey);
    return 0;
  }

  int difference = memcmp(&(pvA->type), &(pvKey->type), sizeof(pvA->type) + pvA->len);
  if (difference) {
    DebugStr(L"Not equal different lengths!");
    DebugPtr(pvA);
    DebugPtr(pvKey);
  } else {
    DebugStr(L"Equal!");
  }
  return !difference;
}

#define STBDS_HASH_TO_ARR(x,elemsize) ((char*) (x) - (elemsize))
#define STBDS_ARR_TO_HASH(x,elemsize) ((char*) (x) + (elemsize))
#define STBDS_FREE(x)   realloc(x,0)

#define stbds_hash_table(a)  ((stbds_hash_index *) stbds_header(a)->hash_table)
 
void stbds_hmfree_func(void *a, size_t elemsize, size_t keyoff)
{
  if (a == NULL) return;
  STBDS_FREE(stbds_header(a)->hash_table);
  STBDS_FREE(stbds_header(a));
}

static ptrdiff_t stbds_hm_find_slot(void *a, size_t elemsize, void *key, size_t keysize, int mode)
{
  void *raw_a = STBDS_HASH_TO_ARR(a,elemsize);
  stbds_hash_index *table = stbds_hash_table(raw_a);
  size_t hash = stbds_hash_bytes(key, keysize,table->seed);
  size_t step = STBDS_BUCKET_LENGTH;
  size_t limit,i;
  size_t pos;
  stbds_hash_bucket *bucket;

  if (hash < 2) hash += 2; // stored hash values are forbidden from being 0, so we can detect empty slots

  pos = hash;

  for (;;) {
    pos &= (table->slot_count-1);
    bucket = &table->storage[pos >> STBDS_BUCKET_SHIFT];

    // start searching from pos to end of bucket, this should help performance on small hash tables that fit in cache
    for (i=pos & STBDS_BUCKET_MASK; i < STBDS_BUCKET_LENGTH; ++i) {
      if (bucket->hash[i] == hash) {
        if (stbds_is_key_equal(a, elemsize, key, keysize, mode, bucket->index[i])) {
          return (pos & ~STBDS_BUCKET_MASK)+i;
        }
      } else if (bucket->hash[i] == STBDS_HASH_EMPTY) {
        return -1;
      }
    }

    // search from beginning of bucket to pos
    limit = pos & STBDS_BUCKET_MASK;
    for (i = 0; i < limit; ++i) {
      if (bucket->hash[i] == hash) {
        if (stbds_is_key_equal(a, elemsize, key, keysize, mode, bucket->index[i])) {
          return (pos & ~STBDS_BUCKET_MASK)+i;
        }
      } else if (bucket->hash[i] == STBDS_HASH_EMPTY) {
        return -1;
      }
    }

    // quadratic probing
    pos += step;
    step += STBDS_BUCKET_LENGTH;
  }
  /* NOTREACHED */
  return -1;
}

void * stbds_hmget_key(void *a, size_t elemsize, void *key, size_t keysize, int mode)
{
  if (a == NULL) {
    // make it non-empty so we can return a temp
    a = stbds_arrgrowf(0, elemsize, 0, 1);
    stbds_header(a)->length += 1;
    memset(a, 0, elemsize);
    stbds_temp(a) = STBDS_INDEX_EMPTY;
    // adjust a to point after the default element
    return STBDS_ARR_TO_HASH(a,elemsize);
  } else {
    stbds_hash_index *table;
    void *raw_a = STBDS_HASH_TO_ARR(a,elemsize);
    // adjust a to point to the default element
    table = (stbds_hash_index *) stbds_header(raw_a)->hash_table;
    if (table == 0) {
      stbds_temp(raw_a) = -1;
    } else {
      ptrdiff_t slot = stbds_hm_find_slot(a, elemsize, key, keysize, mode);
      if (slot < 0) {
        stbds_temp(raw_a) = STBDS_INDEX_EMPTY;
      } else {
        stbds_hash_bucket *b = &table->storage[slot >> STBDS_BUCKET_SHIFT];
        stbds_temp(raw_a) = b->index[slot & STBDS_BUCKET_MASK];
      }
    }
    return a;
  }
}

void * stbds_hmput_default(void *a, size_t elemsize)
{
  // three cases:
  //   a is NULL <- allocate
  //   a has a hash table but no entries, because of shmode <- grow
  //   a has entries <- do nothing
  if (a == NULL || stbds_header(STBDS_HASH_TO_ARR(a,elemsize))->length == 0) {
    a = stbds_arrgrowf(a ? STBDS_HASH_TO_ARR(a,elemsize) : NULL, elemsize, 0, 1);
    stbds_header(a)->length += 1;
    memset(a, 0, elemsize);
    a=STBDS_ARR_TO_HASH(a,elemsize);
  }
  return a;
}

void *stbds_hmput_key(void *a, size_t elemsize, void *key, size_t keysize, int mode)
{
  void *raw_a;
  stbds_hash_index *table;

  if (a == NULL) {
    a = stbds_arrgrowf(0, elemsize, 0, 1);
    memset(a, 0, elemsize);
    stbds_header(a)->length += 1;
    // adjust a to point AFTER the default element
    a = STBDS_ARR_TO_HASH(a,elemsize);
  }

  // adjust a to point to the default element
  raw_a = a;
  a = STBDS_HASH_TO_ARR(a,elemsize);

  table = (stbds_hash_index *) stbds_header(a)->hash_table;

  if (table == NULL || table->used_count >= table->used_count_threshold) {
    stbds_hash_index *nt;
    size_t slot_count;

    slot_count = (table == NULL) ? STBDS_BUCKET_LENGTH : table->slot_count*2;
    nt = stbds_make_hash_index(slot_count, table);
    if (table) {
      STBDS_FREE(table);
    }
    stbds_header(a)->hash_table = table = nt;
  }

  // we iterate hash table explicitly because we want to track if we saw a tombstone
  {
    size_t hash = stbds_hash_bytes(key, keysize,table->seed);
    size_t step = STBDS_BUCKET_LENGTH;
    size_t limit,i;
    size_t pos;
    ptrdiff_t tombstone = -1;
    stbds_hash_bucket *bucket;

    // stored hash values are forbidden from being 0, so we can detect empty slots to early out quickly
    if (hash < 2) hash += 2;

    pos = hash;

    for (;;) {
      pos &= (table->slot_count-1);
      bucket = &table->storage[pos >> STBDS_BUCKET_SHIFT];

      // start searching from pos to end of bucket
      for (i=pos & STBDS_BUCKET_MASK; i < STBDS_BUCKET_LENGTH; ++i) {
        if (bucket->hash[i] == hash) {
          if (stbds_is_key_equal(raw_a, elemsize, key, keysize, mode, i)) {
            stbds_temp(a) = bucket->index[i];
            return STBDS_ARR_TO_HASH(a,elemsize);
          }
        } else if (bucket->hash[i] == 0) {
          pos = (pos & ~STBDS_BUCKET_MASK) + i;
          goto found_empty_slot;
        } else if (tombstone < 0) {
          if (bucket->index[i] == STBDS_INDEX_DELETED)
            tombstone = (ptrdiff_t) ((pos & ~STBDS_BUCKET_MASK) + i);
        }
      }

      // search from beginning of bucket to pos
      limit = pos & STBDS_BUCKET_MASK;
      for (i = 0; i < limit; ++i) {
        if (bucket->hash[i] == hash) {
          if (stbds_is_key_equal(raw_a, elemsize, key, keysize, mode, i)) {
            stbds_temp(a) = bucket->index[i];
            return STBDS_ARR_TO_HASH(a,elemsize);
          }
        } else if (bucket->hash[i] == 0) {
          pos = (pos & ~STBDS_BUCKET_MASK) + i;
          goto found_empty_slot;
        } else if (tombstone < 0) {
          if (bucket->index[i] == STBDS_INDEX_DELETED)
            tombstone = (ptrdiff_t) ((pos & ~STBDS_BUCKET_MASK) + i);
        }
      }

      // quadratic probing
      pos += step;
      step += STBDS_BUCKET_LENGTH;
    }
   found_empty_slot:
    if (tombstone >= 0) {
      pos = tombstone;
      --table->tombstone_count;
    } else {
      ++table->used_count;
    }

    {
      ptrdiff_t i = (ptrdiff_t) stbds_arrlen(a);
    // we want to do stbds_arraddn(1), but we can't use the macros since we don't have something of the right type
      if ((size_t) i+1 > stbds_arrcap(a))
        *(void **) &a = stbds_arrgrowf(a, elemsize, 1, 0);
      raw_a = STBDS_ARR_TO_HASH(a,elemsize);

      STBDS_ASSERT((size_t) i+1 <= stbds_arrcap(a));
      stbds_header(a)->length = i+1;
      bucket = &table->storage[pos >> STBDS_BUCKET_SHIFT];
      bucket->hash[pos & STBDS_BUCKET_MASK] = hash;
      bucket->index[pos & STBDS_BUCKET_MASK] = i-1;
      stbds_temp(a) = i-1;
    }
    return STBDS_ARR_TO_HASH(a,elemsize);
  }
}


void * stbds_hmdel_key(void *a, size_t elemsize, void *key, size_t keysize, size_t keyoffset, int mode)
{
  if (a == NULL) {
    return 0;
  } else {
    stbds_hash_index *table;
    void *raw_a = STBDS_HASH_TO_ARR(a,elemsize);
    table = (stbds_hash_index *) stbds_header(raw_a)->hash_table;
    if (table == 0) {
      return a;
    } else {
      ptrdiff_t slot;
      slot = stbds_hm_find_slot(a, elemsize, key, keysize, mode);
      if (slot < 0)
        return a;
      else {
        stbds_hash_bucket *b = &table->storage[slot >> STBDS_BUCKET_SHIFT];
        int i = slot & STBDS_BUCKET_MASK;
        ptrdiff_t old_index = b->index[i];
        ptrdiff_t final_index = (ptrdiff_t) stbds_arrlen(raw_a)-1-1; // minus one for the raw_a vs a, and minus one for 'last'
        STBDS_ASSERT(slot < (ptrdiff_t) table->slot_count);
        --table->used_count;
        ++table->tombstone_count;
        STBDS_ASSERT(table->used_count >= 0);
        //STBDS_ASSERT(table->tombstone_count < table->slot_count/4);
        b->hash[i] = STBDS_HASH_DELETED;
        b->index[i] = STBDS_INDEX_DELETED;

        // swap delete 
        if (old_index != final_index) {
          // if indices are the same, memcpy is a no-op, but back-pointer-fixup below will fail
          memcpy((char*) a + elemsize*old_index, (char*) a + elemsize*final_index, elemsize);

          // now find the slot for the last element
          slot = stbds_hm_find_slot(a, elemsize,  (char* ) a+elemsize*old_index + keyoffset, keysize, mode);
          STBDS_ASSERT(slot >= 0);
          b = &table->storage[slot >> STBDS_BUCKET_SHIFT];
          i = slot & STBDS_BUCKET_MASK;
          STBDS_ASSERT(b->index[i] == final_index);
          b->index[i] = old_index;
        }
        stbds_header(raw_a)->length -= 1;

        if (table->used_count < table->used_count_shrink_threshold && table->slot_count > STBDS_BUCKET_LENGTH) {
          stbds_header(raw_a)->hash_table = stbds_make_hash_index(table->slot_count>>1, table);
        } else if (table->tombstone_count > table->tombstone_count_threshold) {
          stbds_header(raw_a)->hash_table = stbds_make_hash_index(table->slot_count   , table);
        }

        return a;
      }
    }
  }
  /* NOTREACHED */
  return 0;
}
