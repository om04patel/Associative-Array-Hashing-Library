# Associative Array Hashing Library

This project involves building an associative array tool backed by a hash table, implemented as a library. The goal is to provide efficient key/value storage and retrieval using various hashing and probing strategies. This library demonstrates principles of information hiding and efficient data structure design.

## Project Overview

In this project, we created a library called `libaa.a` that allows users to store and retrieve key/value pairs using an associative array backed by a hash table. The library is accompanied by the `aarray.h` header file, which provides the API for the associative array operations without exposing the underlying hash table implementation.

## Library Design

### Files

- **aarray.h**: Header file containing the API for the associative array operations.
- **hashtools.h**: Header file containing data types and tools for hash table operations.
- **hash-functions.c**: Source file containing the implementations of various hashing and probing functions.
- **hash-table.c**: Source file containing the implementation of the hash table operations such as creating, destroying, inserting, deleting, and querying the table.
- **primes.c**: Source file containing a function to find a prime number for memory allocation based on the requested size.

### Hash Algorithms

We have implemented the following hashing strategies:

1. **Hash by Length**: A simple hashing function that uses the length of the key as the hash value.
2. **Sum of Bytes**: A hashing function that sums the bytes of the key.
3. **Custom Hashing Strategy**: An additional hashing strategy designed and implemented for this assignment. The custom strategy aims to use all the space in the table effectively and avoid clustering.

### Probing Strategies

The probing strategies include linear probing and quadratic probing, with a parameter to report the cost of each probe. This allows us to compute the number of iterations required for each probe, which is useful for analyzing the efficiency of our hashing algorithms.

## User Code and Testing

The user code provided in `mainline.c` allows for various operations, including:

- Interpreting integer keys as binary integers.
- Exploring different sizes of storage tables.
- Choosing the hashing and probing algorithms.
- Performing queries on the table.
- Deleting values from the table.

### Testing Data

The file format for loading data is simple, with lines using a tab character as a divider between the key and the value. Keys that begin with a digit can be interpreted as binary integers if the corresponding option is used.

## How to Build and Use

### Building the Library

To build the library, use the provided `makefile`
