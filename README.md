# UDDSketch -  Uniform DDSketch

A C implementation of the DDSketch algorithm [\[1\]][1] for quantile estimation with the following additions:

- uniform collapsing strategy, as described in [\[2\]][2];
- additional sketch implementation for dealing with negative values.

The code is inspired by the Java implementation of DDSketch [\[3\]][4] from [Datadog, Inc.][3]  and make use of the `libdict` C library from Farooq Mela [\[4\]][5].

# Build
Tested on Linux and macOS, it should work on other UNIX-like OSes.

You need CMake 3.15 or higher and build tools installed.

Run the script `build.sh`. At the end, a sub-directory named 'install' will be created in which you will find the binary executable.


# Version history

- v1.3 fixed a bug related to an incorrect handling of zero values in some cases
- v1.2.1 current version: fixed a regression issue introduced in v1.2
- v1.2 fixed some additional memory leaks arising when removing items
- v1.1 fixed some memory leaks
- v1.0 initial release


# Usage

The executable can be run as follows:

uddsketch-test -f <input filaname> -b <buckets> -a <alpha> -s <store> -n -o

In particular:

- the input file must be a binary file storing an array of doubles; this
  parameter is mandatory;
- the number of buckets is the maximum number of buckets except when the store
  selected with -s is the unbounded map store (i.e., -s 0) in which case it is
  ignored; in the other cases, the default value is 256;
- alpha is the desired accuracy parameter; the default value is 0.01;
- store is a parameter related to different implementations:

  s = 0 unbounded map store (no collapses, implements DDSketch);
  s = 1 collapsing last two buckets map store (note: the last two buckets are
  not necessarily adjacent, implements DDSketch);
  s = 2 collapsing first two buckets map store (note: the first two buckets are
  not necessarily adjacent, implements DDSketch);
  s = 3 uniform collapse map store (implements UDDSketch);
  s = 4 uniform collapse map store with initial alpha value set by the code considering a predefined number of collapses (implements UDDSketch);
  i.e., the alpha value set by -a is the desired target accuracy and the code
  automatically selects an initial alpha value so that the desired target alpha
  value is not exceeded (warning: this is true only if the number of buckets is
  large enough; see the paper for additional details;

  the default value is 0 (unbounded map store);

- the parameter -n is required ONLY if the input includes negative values;
- the parameter -o is required ONLY if the output is desired in csv format.

### Notes
- Although this implementation of UDDSketch can handle element removals, the test code in uddsketch-test does not deal with this case.
  If your code also remove items, remember that you need to compute exact quantiles on the final stream resulting from insertions and deletions, not on the initial input!
  Otherwise, when you compare exact versus approximate quantiles output from UDDSketch, the results will not be correct.
- This implementation may not work with alpha values that are too small (less than 10^-6).

# References
\[1\] **Charles Masson, Jee E. Rim and Homin K. Lee**. *DDSketch: a fast and fully-mergeable quantile sketch with relative-error guarantees*. Proceedings of the VLDB Endowment 12.12 (2019): 2195-2205.

\[2\] **Italo Epicoco, Catiuscia Melle, Massimo Cafaro, Marco Pulimeno, Giuseppe Morleo.** *UDDSketch: Accurate Tracking of Quantiles in Data Streams.*


[1]: <http://www.vldb.org/pvldb/vol12/p2195-masson.pdf> "DDSketch: A Fast and fully-Mergeable Quantile Sketch with Relative-Error Guarantees"

[2]: <> "UDDSketch: Accurate Tracking of Quantiles in Data Streams"

[3]: <https://datadoghq.com> "Datadog, Inc."

[4]: <https://github.com/DataDog/sketches-java> "ddsketch Java"

[5]: <https://github.com/fmela/libdict> "libdict C library"
