# Hash Branch and Bound Skyline
This project implements Branch and Bound Skyline.
To efficient calculate the skyline in high dimensionality datasets
the R-Tree was replaced with hashing algorithms. 
Four state-of-the-art hashing algorithms was used here instead of R-Tree.

In the following paper there are details regarding performance
and experimentation of different skyline calculation strategies.
The results of this study shows that calculating the skyline of
hign dimensional data (i.e. image descriptor vectors) is better
handled when using hashing structures as partitioning method.

Nikolaos Georgiadis, Eleftherios Tiakas, and Yannis Manolopoulos. 2017. 
Detecting Intrinsic Dissimilarities in Large Image Databases through Skylines. 
In Proceedings of the 9th International Conference on Management of Digital EcoSystems (MEDES '17). 
ACM, New York, NY, USA, 194-201. 
DOI: https://doi.org/10.1145/3167020.3167050


Implementation of Branch and Bound Skyline is based on the following paper:

Dimitris Papadias, Yufei Tao, Greg Fu, and Bernhard Seeger. 2005. 
Progressive skyline computation in database systems. 
ACM Trans. Database Syst. 30, 1 (March 2005), 41-82. 
DOI: https://doi.org/10.1145/1061318.1061320


All hashing algorithms implemented in this project 
were based on the following repository:

https://github.com/willard-yuan/hashing-baseline-for-image-retrieval
