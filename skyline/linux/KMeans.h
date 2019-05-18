struct KMeansOptions
{
	int MaxIterations = 10;
};

#pragma once
class KMeans
{
	//number of clusters
	uword _k;

	//data
	mat _data;

	//cluster index for each row 
	uvec _cluster_index;

	//means
	mat _means;

	KMeansOptions _kmeans_options;

	double distance(vec a, vec b);
	double distance2(vec a, vec b);
	vec getMean(mat a, uvec indices);

	double getDistance(colvec a, colvec b);
	double getLabel(mat centroids, colvec pt);

public:
	KMeans(uword k, mat data);
	~KMeans();
	
	mat Cluster_OLD();
	mat Cluster2();
	uvec GetClusterIndex();
};

