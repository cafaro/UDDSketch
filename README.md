# UDDSketch -  Uniform DDSketch

A C implementation of the DDSketch algorithm [\[1\]][1] for quantile estimation with the following additions:

- uniform collapsing strategy, as described in [\[2\]][2];
- additional sketch implementation for dealing with negative values.

The code is inspired by the Java implementation of DDSketch [\[3\]][4] from [Datadog, Inc.][3]  and make use of the `libdict` C library from Farooq Mela [\[4\]][5].

# Build
Tested on Linux, but it should work on MacOS and other UNIX-like OSes.

You need CMake 3.15 or higher and build tools installed. 

Run the script `build.sh`. If everything goes fine, a sub-directory named 'install' will be created with all of the binaries.

# References
\[1\] **Charles Masson, Jee E. Rim and Homin K. Lee**. *DDSketch: a fast and fully-mergeable quantile sketch with relative-error guarantees*. Proceedings of the VLDB Endowment 12.12 (2019): 2195-2205.

\[2\] **I. Epicoco, C. Melle, M. Cafaro, M. Pulimeno and G. Morleo**. UDDSketch: Accurate Tracking of Quantiles in Data Streams. IEEE Access, vol. 8, pp. 147604-147617, 2020, ISSN: 2169-3536, DOI: 10.1109/ACCESS.2020.3015599


[1]: <http://www.vldb.org/pvldb/vol12/p2195-masson.pdf> "DDSketch: A Fast and fully-Mergeable Quantile Sketch with Relative-Error Guarantees"

[2]: <> "UDDSketch: Accurate Tracking of Quantiles in Data Streams"

[3]: <https://datadoghq.com> "Datadog, Inc."

[4]: <https://github.com/DataDog/sketches-java> "ddsketch Java"

[5]: <https://github.com/fmela/libdict> "libdict C library"
