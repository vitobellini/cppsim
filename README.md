# cppsim
Fast multi-threaded memory optimized tool to compute cosine similarity on very large matrices imported from NumPy

## Requirements

* CMake 3.7
* Boost lib 1.66
* ZLib

## Installing

```
cmake .
```

## Run

Using cppsim is very straightforward.

```
$ ./cppsim 
Allowed options:
  -h [ --help ]         print usage message
  -i [ --input ] arg    pathname for input matrix
  -o [ --output ] arg   pathname for output matrix
  -d [ --dir ] arg      output directory for row vectors
  -s [ --split ]        split matrix into row vectors
  -u [ --users ]        path for users index file to compute similarities
  -t [ --threads ] arg  threads
```

There are two main ways to operate with cppsim.
It is possible to compute the similarity matrix or split it into row vectors.

The former method needs the input and output matrices, as shown in the following example.

```
$ ./cppsim -i input.npy -o output.npy
```

The latter splits the matrix similarity into row vectors, one for each file. This method is preferred when the RAM you have is not enough to maintain the full similarity matrix. To split the output matrix into row vectors, run cppsim with -s parameter and specify with -d the directory in which to store the row vectors. 

```
$ ./cppsim -i input.npy -s -d similarities 
```

If a users index file is specified, cppsim must be run with the -s option. In this case, it will compute similarities only for the specified users.