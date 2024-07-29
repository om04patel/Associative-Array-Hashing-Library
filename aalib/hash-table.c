#include <stdio.h>   
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "hashtools.h"

/** forward declaration */
static HashAlgorithm lookupNamedHashStrategy(const char *name);
static HashProbe lookupNamedProbingStrategy(const char *name);

/**
 * Create a hash table of the given size,
 * which will use the given algorithm to create hash values,
 * and the given probing strategy
 *
 *  @param  hash  the HashAlgorithm to use
 *  @param  probingStrategy algorithm used for probing in the case of
 *				collisions
 *  @param  newHashSize  the size of the table (will be rounded up
 *				to the next-nearest larger prime, but see exception)
 *  @see         HashAlgorithm
 *  @see         HashProbe
 *  @see         Primes
 *
 *  @throws java.lang.IndexOutOfBoundsException if no prime number larger
 *		              		than newHashSize can be found (currently only primes
 *				less than 5000 are known)
 */
AssociativeArray *
aaCreateAssociativeArray(
		size_t size,
		char *probingStrategy,
		char *hashPrimary,
		char *hashSecondary
	)
{
	AssociativeArray *newTable;

	newTable = (AssociativeArray *) malloc(sizeof(AssociativeArray));

	newTable->hashAlgorithmPrimary = lookupNamedHashStrategy(hashPrimary);
	newTable->hashNamePrimary = strdup(hashPrimary);
	newTable->hashAlgorithmSecondary = lookupNamedHashStrategy(hashSecondary);
	newTable->hashNameSecondary = strdup(hashSecondary);
	newTable->hashProbe = lookupNamedProbingStrategy(probingStrategy);
	newTable->probeName = strdup(probingStrategy);

	newTable->size = getLargerPrime(size);

	if (newTable->size < 1) {
		fprintf(stderr, "Cannot create table of size %ld\n", size);
		free(newTable);
		return NULL;
	}

	newTable->table = (KeyDataPair *) malloc(newTable->size * sizeof(KeyDataPair));

	/** initialize everything with zeros */
	memset(newTable->table, 0, newTable->size * sizeof(KeyDataPair));

	newTable->nEntries = 0;

	newTable->insertCost = newTable->searchCost = newTable->deleteCost = 0;

	return newTable;
}

/**
 * deallocate all the memory in the store -- the keys (which we allocated),
 * and the store itself.
 * The user * code is responsible for managing the memory for the values
 */
void
aaDeleteAssociativeArray(AssociativeArray *aarray)
{
	/**
	 * TO DO:  clean up the memory managed by our utility
	 *
	 * Note that memory for keys are managed, values are the
	 * responsibility of the user
	 */
	     // Free memory for the hash table
    if(aarray == NULL){
	return;
	}
	for(int i = 0; i < aarray->size; i++){
		if(aarray->table[i].validity == HASH_USED || aarray->table[i].validity == HASH_DELETED){
                    if(aarray->table[i].key != NULL){	
			free(aarray->table[i].key);
}
		}}
	

    // Free memory for hash strategy names
    free(aarray->hashNamePrimary);
    free(aarray->hashNameSecondary);
    free(aarray->probeName);
    free(aarray->table);
    free(aarray);
}

/**
 * iterate over the array, calling the user function on each valid value
 */
int aaIterateAction(
		AssociativeArray *aarray,
		int (*userfunction)(AAKeyType key, size_t keylen, void *datavalue, void *userdata),
		void *userdata
	)
{
	int i;

	for (i = 0; i < aarray->size; i++) {
		if (aarray->table[i].validity == HASH_USED) {
			if ((*userfunction)(
					aarray->table[i].key,
		             		aarray->table[i].keylen,
					aarray->table[i].value,
					userdata) < 0) {
				return -1;
			}
		}
	}
	return 1;
}

/** utilities to change names into functions, used in the function above */
static HashAlgorithm lookupNamedHashStrategy(const char *name)
{
	if (strncmp(name, "sum", 3) == 0) {
		return hashBySum;
	} else if (strncmp(name, "len", 3) == 0) {
		return hashByLength;
	}
        else if (strncmp(name, "custom", 6) == 0) {
        return customHash;
    }
		// TO DO: add in your own strategy here

	fprintf(stderr, "Invalid hash strategy '%s' - using 'sum'\n", name);
	return hashBySum;
}

static HashProbe lookupNamedProbingStrategy(const char *name)
{
	if (strncmp(name, "lin", 3) == 0) {
		return linearProbe;
	}else if (strncmp(name, "qua", 3) == 0) {
		return quadraticProbe;
	}else if (strncmp(name, "dou", 3) == 0) {
		return doubleHashProbe;
	}

	fprintf(stderr, "Invalid hash probe strategy '%s' - using 'linear'\n", name);
	return linearProbe;
}

/**
 * Add another key and data value to the table, provided there is room.
 *
 *  @param  key  a string value used for searching later
 *  @param  value a data value associated with the key
 *  @return      the location the data is placed within the hash table,
 *				 or a negative number if no place can be found
 */
int aaInsert(AssociativeArray *aarray, AAKeyType key, size_t keylen, void *value)
{
	/**
	 * TO DO:  Search for a location where this key can go, stopping
	 * if we find a value that has been delete and reuse it.
	 *
	 * If a suitable location is found, we then initialize that
	 * slot with the new key and data
	 */
        // Initialize the cost counter
// Compute the initial hash index using the primary hash function
    HashIndex index = aarray->hashAlgorithmPrimary(key, keylen, aarray->size);
    
    // Initialize insert cost and record the initial index
    int cost = 0;
    HashIndex initialindex = index;

    // Allocate memory for a copied key
    AAKeyType copiedKey = malloc(keylen + 1);
    if (copiedKey == NULL) {
        return -1; // Memory allocation failure
    }
    memcpy(copiedKey,key,keylen);
    copiedKey[keylen] = '\0';

    // Copy the key and null-terminate the copied key

    while (1) {
        // Check if the current slot is empty (0) or has a tombstone (-1).
        if (aarray->table[index].validity == HASH_EMPTY || aarray->table[index].validity == HASH_DELETED) {
            // Insert the key, value, and update metadata
            aarray->table[index].key = copiedKey;
            aarray->table[index].keylen = keylen;
            aarray->table[index].value = value;
            aarray->table[index].validity = HASH_USED;
            aarray->nEntries++;
			cost++;
			aarray->insertCost += cost;
            
           // free(copiedKey);         // Update insert cost and free the copied key memory before returning
            return index;
        }

        // Use the hash probing strategy to find the next available slot
        index = aarray->hashProbe(aarray, key, keylen, index, 1, &aarray->insertCost);

        // If we have cycled through the entire table and haven't found an empty slot, return -1
        if (index == initialindex) {
			aarray->insertCost += cost;
            return -1;
        }
    }
	free(copiedKey);
}



/**
 * Locates the KeyDataPair associated with the given key, if
 * present in the table.
 *
 *  @param  key  the key to search for
 *  @return      the KeyDataPair containing the key, if the key
 *				 was present in the table, or NULL, if it was not
 *  @see         KeyDataPair
 */
void *aaLookup(AssociativeArray *aarray, AAKeyType key, size_t keylen)
{
    // Compute the initial hash index using the primary hash function
    HashIndex index = aarray->hashAlgorithmPrimary(key, keylen, aarray->size);
    int startIndex = index;
    int cost = 0;

    while (aarray->table[index].validity != HASH_EMPTY)
    {
        // Check if the current slot is marked as deleted
        if (aarray->table[index].validity == HASH_DELETED)
        {
            // This means the key is not in the table
            index++;
	    cost++;
	    aarray->searchCost += cost;
            if (index == startIndex)
            {
                return NULL; // The entire table has been searched, key not found
            }
            continue;
        }

        // Check if the current slot matches the key
        if (aarray->table[index].keylen == keylen && memcmp(aarray->table[index].key, key, keylen) == 0)
        {
            return aarray->table[index].value; // Key found, return the associated value
        }

        // If the current slot doesn't match the key, handle collision (optional):
        // You can use the same probing strategy as in aaInsert to find the next slot
        index++;
	cost++;
	aarray->searchCost += cost;
        if (index == startIndex)
        {
            return NULL; // The entire table has been searched, key not found
        }
    }

    // Key not found
    return NULL;
}


/**
 * Locates the KeyDataPair associated with the given key, if
 * present in the table.
 *
 *  @param  key  the key to search for
 *  @return      the index of the KeyDataPair containing the key,
 *				 if the key was present in the table, or (-1),
 *				 if no key was found
 *  @see         KeyDataPair
 */
void *aaDelete(AssociativeArray *aarray, AAKeyType key, size_t keylen)
{
	/**
	 * TO DO: Deletion is closely related to lookup;
	 * you must find where the key is stored before
	 * you delete it, after all.
	 *
	 * Implement a deletion algorithm based on tombstones,
	 * as described in class
	 */
    int cost = 0;
	HashIndex index = aarray->hashAlgorithmPrimary(key, keylen, aarray->size);
    HashIndex initialIndex = index;  // Save the initial index for cost calculation

    while (1) {
        if (aarray->table[index].validity == HASH_USED) {
            // Check if the current slot contains the key we're looking for.
            if (aarray->table[index].keylen == keylen && memcmp(aarray->table[index].key, key, keylen) == 0) {
                //printf("Key found at index %lu; marking the slot as deleted...\n", index);
                aarray->table[index].validity = HASH_DELETED;
                                aarray->nEntries--;
				cost++;
                aarray->deleteCost += cost;
                return aarray->table[index].value;
            }
        } else if (aarray->table[index].validity == HASH_EMPTY) {
            //printf("Reached an empty slot at index %lu; key is not in the table.\n", index);
			cost++;
                        aarray->nEntries--;
            aarray->deleteCost += 1;
            return NULL;
        }

        // Move to the next slot using the probing strategy.
        index = aarray->hashProbe(aarray, key, keylen, index, 1, &aarray->deleteCost);
        aarray->deleteCost++;  
        if (index == initialIndex) {
			cost++;
        	aarray->deleteCost += 1;

	/**
	 * TO DO: Deletion is closely related to lookup;
	 * you must find where the key is stored before
	 * you delete it, after all.
	 *
	 * Implement a deletion algorithm based on tombstones,
	 * as described in class
	 */

	return NULL;
}}
}

/**
 * Print out the entire aarray contents
 */
void aaPrintContents(FILE *fp, AssociativeArray *aarray, char * tag)
{
	char keybuffer[128];
	int i;

	fprintf(fp, "%sDumping aarray of %d entries:\n", tag, aarray->size);
	for (i = 0; i < aarray->size; i++) {
		fprintf(fp, "%s  ", tag);
		if (aarray->table[i].validity == HASH_USED) {
			printableKey(keybuffer, 128,
					aarray->table[i].key,
					aarray->table[i].keylen);
			fprintf(fp, "%d : in use : '%s'\n", i, keybuffer);
		} else {
			if (aarray->table[i].validity == HASH_EMPTY) {
				fprintf(fp, "%d : empty (NULL)\n", i);
			} else if ( aarray->table[i].validity == HASH_DELETED) {
				printableKey(keybuffer, 128,
						aarray->table[i].key,
						aarray->table[i].keylen);
				fprintf(fp, "%d : empty (deleted - was '%s')\n", i, keybuffer);
			} else {
				fprintf(fp, "%d : invalid validity state %d\n", i,
						aarray->table[i].validity);
			}
		}
	}
}


/**
 * Print out a short summary
 */
void aaPrintSummary(FILE *fp, AssociativeArray *aarray)
{
	fprintf(fp, "Associative array contains %d entries in a table of %d size\n",
			aarray->nEntries, aarray->size);
	fprintf(fp, "Strategies used: '%s' hash, '%s' secondary hash and '%s' probing\n",
			aarray->hashNamePrimary, aarray->hashNameSecondary, aarray->probeName);
	fprintf(fp, "Costs accrued due to probing:\n");
	fprintf(fp, "  Insertion : %d\n", aarray->insertCost);
	fprintf(fp, "  Search    : %d\n", aarray->searchCost);
	fprintf(fp, "  Deletion  : %d\n", aarray->deleteCost);
}

