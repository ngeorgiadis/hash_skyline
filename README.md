# Hash Branch and Bound Skyline
This project implements Branch and Bound Skyline.
To efficient calculate the skyline in high dimensionality datasets
the R-Tree has been replaced with hashing algorithms. 
Four state-of-the-art hashing algorithms are used here instead of R-Tree.

In the following paper there are details regarding performance
and experimentation of different skyline calculation algorithms.
Results of this study shows that hashing algorithms are more efficient 
in high dimensional space.

Nikolaos Georgiadis, Eleftherios Tiakas, and Yannis Manolopoulos. 2017. 
Detecting Intrinsic Dissimilarities in Large Image Databases through Skylines. 
In Proceedings of the 9th International Conference on Management of Digital EcoSystems (MEDES '17). 
ACM, New York, NY, USA, 194-201. DOI: https://doi.org/10.1145/3167020.3167050


Implementation of Branch and Bound Skyline is based on the following paper:

Dimitris Papadias, Yufei Tao, Greg Fu, and Bernhard Seeger. 2005. 
Progressive skyline computation in database systems. 
ACM Trans. Database Syst. 30, 1 (March 2005), 41-82. 
DOI=http://dx.doi.org/10.1145/1061318.1061320


All hashing algorithms implemented in this project 
were based on the following repository:

https://github.com/willard-yuan/hashing-baseline-for-image-retrieval
