/**
 * Mosait by Tangui Morlier
 * Under GPLv3
 */
#ifndef MOSAIT_CENTROID
#define MOSAIT_CENTROID
#include "MosaitCommon.h"
#include "MosaitImg.h"
#include <vector>
#define MOSAITCENTROID_REMOVE 1
#define MOSAITCENTROID_UNFAIR 2
#define MOSAITCENTROID_KEEP 0

class MosaitCentroid
{
	public:
		MosaitCentroid();
		MosaitCentroid(MosaitCentroid & centroid, int nb_sub_centroids, int max = 0, string out = "");
		MosaitCentroid(vector<MosaitImg> & images);
		MosaitCentroid(vector<MosaitImg> & vimages, int nb_sub_centroids, int max);
		MosaitImg getClosestImage(MosaitImg & image, int remove_or_unfair_it = MOSAITCENTROID_REMOVE);
		bool operator==(const MosaitCentroid & centroid) const;
		std::ostream& dumpInStream(std::ostream& os);
		friend std::ostream& operator<<(std::ostream& os, MosaitCentroid & mc);
		friend istream & operator >>(istream &ins, MosaitCentroid & mc);
		int getNbImages() const;
		void printNbImages(string prefix = "") const;
		void dumpInFile(string & file);
		void split(int nb_sub_centroids, int max = 0);
		static bool compareBySize(const MosaitCentroid & a, const MosaitCentroid & b) ;
//		static bool compareBySubImageDistance(const MosaitCentroid & a, const MosaitCentroid & b) ;
		inline static bool compareBySubImageDistance(const MosaitCentroid & a, const MosaitCentroid & b) 
		{
			return a.virtual_img.getDistance() < b.virtual_img.getDistance();
		};
		int getNbCentroids() const;
	private:
		MosaitImg virtual_img;
		vector<MosaitImg> images;
		vector<MosaitCentroid> sub_centroids;
		vector<MosaitCentroid>::iterator getClosestCentroid(MosaitImg & image);
		void updateVirtualImage();
		void addImage(MosaitImg& image);
		void init(vector<MosaitImg> & images, int max = 0);
		void clearImages();
		bool has_sub_centroids;
		int removed;
		string out_file;

};

#endif
