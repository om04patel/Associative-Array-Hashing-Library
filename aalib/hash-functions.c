#include <stdio.h>
#include <string.h> // for strcmp()
#include <ctype.h> // for isprint()

#include "hashtools.h"

/** check if the two keys are the same */
int
doKeysMatch(AAKeyType key1, size_t key1len, AAKeyType key2, size_t key2len)
{
	/** iff the lengths don't match, the keys can't match */
	if (key1len != key2len)
		return 0;

	return memcmp(key1, key2, key1len) == 0;
}

/* provide the hex representation of a value */
static char toHex(int val)
{
	if (val < 10) return (char) ('0' + val);
	return (char) ('a' + (val - 10));
}

/**
 * Provide the key in a printable form.  Uses a static buffer,
 * which means that not only is this not thread-safe, but
 * even runs into trouble if called twice in the same printf().
 *
 * That said, is does provide a memory clean way to give a 
 * printable string return value to the calling code
 */
int
printableKey(char *buffer, int bufferlen, AAKeyType key, size_t printlen)
{
    int i, allChars = 1;
    char *loadptr;


    for (i = 0; allChars && i < printlen; i++) {
        if ( ! isprint(key[i])) allChars = 0;
    }

    if (allChars) {
        snprintf(buffer, bufferlen, "char key:[");
        loadptr = &buffer[strlen(buffer)];
        for (i = 0; i < printlen && loadptr - buffer < bufferlen - 2; i++) {
            *loadptr++ = key[i];
        }
        *loadptr++ = ']';
        *loadptr++ = 0;
    } else {
        snprintf(buffer, bufferlen, "hex key:[0x");
        loadptr = &buffer[strlen(buffer)];
        for (i = 0; i < printlen && loadptr - buffer < bufferlen - 4; i++) {
            *loadptr++ = toHex((key[i] & 0xf0) >> 4); // top nybble -> first hext digit
            *loadptr++ = toHex(key[i] & 0x0f);        // bottom nybble -> second digit
        }
        *loadptr++ = ']';
        *loadptr++ = 0;
    }
    return 1;
}
/**
 * Calculate a hash value based on the length of the key
 *
 * Calculate an integer index in the range [0...size-1] for
 * 		the given string key
 *
 *  @param  key  key to calculate mapping upon
 *  @param  size boundary for range of allowable return values
 *  @return      integer index associated with key
 *
 *  @see    HashAlgorithm
 */
HashIndex hashByLength(AAKeyType key, size_t keyLength, HashIndex size)
{
	return keyLength % size;
}


HashIndex customHash(AAKeyType key, size_t keylen, HashIndex tableSize) {
    HashIndex hash = 0;

    for (size_t i = 0; i < keylen; i++) {
        hash = (hash * 31 + (HashIndex)key[i]) % tableSize;
    }
    return hash;
}
/**
 * Calculate a hash value based on the sum of the values in the key
 *
 * Calculate an integer index in the range [0...size-1] for
 * 		the given string key, based on the sum of the values
 *		in the key
 *
 *  param  key  key to calculate mapping upon
 *  param  size boundary for range of allowable return values
 *  return      integer index associated with key
 */
HashIndex hashBySum(AAKeyType key, size_t keyLength, HashIndex size)
{
	HashIndex sum = 0;
    

	/**
	 * TO DO: you will need to implement a summation based
	 * hashing algorithm here, using a sum-of-bytes
	 * strategy such as that discussed in class.  Take
	 * a look at HashByLength if you want an example
	 * of a "working" (but not very smart) hashing
	 * algorithm.
	 */
	for(size_t i = 0; i < keyLength; i++){
		sum += (HashIndex)key[i]; 
	}
	return sum % size;
}
/**
 * Locate an empty position in the given array, starting the
 * search at the indicated index, and restricting the search
 * to locations in the range [0...size-1]
 *
 *  @param  index where to begin the search
 *  @param  AssociativeArray associated AssociativeArray we are probing
 *  @param  invalidEndsSearch should the identification of a
 *				KeyDataPair marked invalid end our search?
 *				This is true if we are looking for a location
 *				to insert new data
 *  @return index of location where search stopped, or -1 if
 *				search failed
 *
 *  @see    HashProbe
 */
HashIndex linearProbe(AssociativeArray *hashTable,
		AAKeyType key, size_t keylength,
		int index, int invalidEndsSearch, int *cost
	)
{
	/**
	 * TO DO: you will need to implement an algorithm
	 * that probes until it finds an "empty" slot in
	 * the hashTable.  Note that because of tombstones,
	 * there are multiple ways of a slot being empty.
	 * Additionally, the code in HashTable depends on
	 * this code to find an actually empty slot, so
	 * this code is called with the results of the
	 * hash -- this means that the "index" value may
	 * already be valid on entry.
	 *
	 * Note that if an empty place cannot be found,
	 * you are to return (-1).  If a zero or positive
	 * value is returned, the calling code <i>will</i>
	 * use it, so be sure your return values are correct!
	 *
	 * For this routine, implement a "linear" probing
	 * strategy, such as that discussed in class.
	 */
    // Initial step size for linear probing (1 means moving to the next slot)
   int j = index;

    while (hashTable->table[j].validity != HASH_EMPTY &&
           (invalidEndsSearch || hashTable->table[j].validity == HASH_DELETED)) {
            j = (j + 1) % hashTable->size;
            (*cost)++;

        // If we have cycled through the entire table, and it's not empty,
        // then the table is full, and we cannot insert.
        if (j == index) {
            fprintf(stderr, "Hash table full\n");
            return -1;
        }
    }
    return j;
}

/**
 * Locate an empty position in the given array, starting the
 * search at the indicated index, and restricting the search
 * to locations in the range [0...size-1]
 *
 *  @param  index where to begin the search
 *  @param  hashTable associated HashTable we are probing
 *  @param  invalidEndsSearch should the identification of a
 *				KeyDataPair marked invalid end our search?
 *				This is true if we are looking for a location
 *				to insert new data
 *  @return index of location where search stopped, or -1 if
 *				search failed
 *
 *  @see    HashProbe
 */
HashIndex quadraticProbe(AssociativeArray *hashTable, AAKeyType key, size_t keylen,
    int startIndex, int invalidEndsSearch, int *cost)
{
    int s = 0;  // Quadratic probing counter
    int j = startIndex;

    // Continue probing until an empty slot or a deleted slot (tombstone) is found, or the entire table is probed
    while (hashTable->table[j].validity != HASH_EMPTY &&
           (invalidEndsSearch || hashTable->table[j].validity == HASH_DELETED))
    {
        s++;
        j = (startIndex + s * s) % hashTable->size;  // Quadratic probing formula
        (*cost)++;

        // If we have probed the entire table without finding an empty or deleted slot,
        // then the table is full, and we cannot insert.
        if (s == hashTable->size)
        {
           fprintf(stderr, "Hash table full\n");
           return -1;
        }
    }

    return j;  // Return the index of the empty or deleted slot found during probing
}
 
	



/**
 * Locate an empty position in the given array, starting the
 * search at the indicated index, and restricting the search
 * to locations in the range [0...size-1]
 *
 *  @param  index where to begin the search
 *  @param  hashTable associated HashTable we are probing
 *  @param  invalidEndsSearch should the identification of a
 *				KeyDataPair marked invalid end our search?
 *				This is true if we are looking for a location
 *				to insert new data
 *  @return index of location where search stopped, or -1 if
 *				search failed
 *
 *  @see    HashProbe
 */
	/**
	 * TO DO: you will need to implement an algorithm
	 * that calls a second hash function (listed
	 * in the hashTable) and uses the value obtained
	 * as a result from that as the step size.
	 *
	 * Beyond that, the algorithm proceeds as with
	 * the above strategies.
	 */
HashIndex doubleHashProbe(AssociativeArray *hashTable, AAKeyType key, size_t keylen, int startIndex, int invalidEndsSearch, int *cost)
{
    /**
     * TO DO: you will need to implement an algorithm
     * that calls a second hash function (listed
     * in the hashTable) and uses the value obtained
     * as a result from that as the step size.
     *
     * Beyond that, the algorithm proceeds as with
     * the above strategies.
     */

    // Get the step size from the second hash function
    HashIndex stepSize = hashTable->hashAlgorithmSecondary(key, keylen, hashTable->size);

    // Initialize the starting index for probing
    int j = startIndex;
    int s = 0; // Counter for the number of probes

    while (hashTable->table[j].validity != HASH_EMPTY &&
           (invalidEndsSearch || hashTable->table[j].validity == HASH_DELETED))
    {
        // Update the step size based on the result of the second hash function
        s++;
        j = (startIndex + s * stepSize) % hashTable->size;
        (*cost)++;

        // If we have cycled through the entire table, and it's not empty,
        // then the table is full, and we cannot insert.
        if (s == hashTable->size)
        {
            fprintf(stderr, "Hash table full\n");
            return -1;
        }
    }

    return j;
}
