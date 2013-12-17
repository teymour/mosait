/**
 * Mosait by Tangui Morlier
 * Under GPLv3
 */
#include "MosaitCentroid.h"
#include <iostream>
#include <fstream>
MosaitCentroid::MosaitCentroid() : 
	virtual_img(DEF_IMG_X, DEF_IMG_Y), has_sub_centroids(false), sub_centroids()
{
}

void MosaitCentroid::clearImages()
{
	images.clear();
}

MosaitCentroid::MosaitCentroid(vector<MosaitImg> & vimages) : has_sub_centroids(false), virtual_img(DEF_IMG_X, DEF_IMG_Y), images(vimages), sub_centroids()
{
}

MosaitCentroid::MosaitCentroid(vector<MosaitImg> & vimages, int nb_sub_centroids, int max) : 
	virtual_img(DEF_IMG_X, DEF_IMG_Y), has_sub_centroids(true), sub_centroids(nb_sub_centroids, MosaitCentroid())
{
	init(vimages, max);
}

MosaitCentroid::MosaitCentroid(MosaitCentroid & centroid, int nb_sub_centroids, int max, string out) :
        virtual_img(DEF_IMG_X, DEF_IMG_Y), has_sub_centroids(true), sub_centroids(nb_sub_centroids, MosaitCentroid()), out_file(out)
{
        init(centroid.images, max);
}



void MosaitCentroid::init(vector<MosaitImg> & vimages, int max)
{
	for (vector<MosaitCentroid>::iterator iCent = sub_centroids.begin(); iCent < sub_centroids.end() ; iCent++)
		iCent->virtual_img = MosaitImg(DEF_IMG_X, DEF_IMG_Y);
	
	float distance, best_distance = 10;
	vector<MosaitCentroid>::iterator best_centroid;
	vector<MosaitCentroid> last_centroids;
	int supercpt = 0;
	for(int i = 0 ; 1 ; i++) {
		cerr << "iteration " << i << endl;
		for (vector<MosaitCentroid>::iterator iCent = sub_centroids.begin() ; iCent < sub_centroids.end() ; iCent++) {
			iCent->clearImages();
		}
		int y = 0;
		for (vector<MosaitImg>::iterator iImg = vimages.begin() ; iImg < vimages.end() ; iImg++) {
			vector<MosaitCentroid>::iterator lastiCent = sub_centroids.end();
			for(int x = 0 ; x < 1 ; x++) {
			  best_distance = 10000;
			  best_centroid = sub_centroids.end();
//			  cerr << "image "<< y++ << endl;
			  for (vector<MosaitCentroid>::iterator iCent = sub_centroids.begin() ; iCent < sub_centroids.end() ; iCent++) {
				if (lastiCent == iCent)
					continue;
				distance = iImg->getDistanceWith(iCent->virtual_img, false);
				if (distance < best_distance) {
					best_distance = distance;
					best_centroid = iCent;
				}
//*/
//				iCent->virtual_img.setDistanceWith(*iImg, false);
			  }
//			  sort(sub_centroids.begin(), sub_centroids.end(), MosaitCentroid::compareBySubImageDistance);
			  best_centroid->addImage(*iImg);
			  lastiCent = best_centroid;
			}
		}
		if (sub_centroids == last_centroids || i >= 25) {
			sort(sub_centroids.begin(), sub_centroids.end(), MosaitCentroid::compareBySize);
			if (sub_centroids.begin()->getNbImages())
				break;
			sub_centroids.front().images.insert(sub_centroids.front().images.begin(),
						sub_centroids.back().images.begin() +  sub_centroids.back().images.size()/2, 
						sub_centroids.back().images.end() );
			sub_centroids.back().images = vector<MosaitImg>(sub_centroids.back().images.begin(), 
						sub_centroids.back().images.begin() +  sub_centroids.back().images.size()/2);
			i = 0;			
		}
		last_centroids = sub_centroids;
		for (vector<MosaitCentroid>::iterator iCent = sub_centroids.begin() ; iCent < sub_centroids.end() ; iCent++) 
		{
			iCent->updateVirtualImage();
		}
		if (out_file.length()) 
			 dumpInFile(out_file);
		printNbImages();
	}
	if (!max)
		return;
	cerr << "max: " << max << endl;
	for(vector<MosaitCentroid>::iterator iCent = sub_centroids.end() -1 ; iCent >= sub_centroids.begin() ; iCent--) 
	{
		if (max > iCent->getNbImages() )	
			break;
		iCent->split(iCent->getNbImages() / max +1, max);
		
	}
}
void MosaitCentroid::printNbImages(string prefix) const
{
	if (!has_sub_centroids)
	{
		cerr << getNbImages() << "\t";
		return;
	}
	cerr << getNbImages() << endl << prefix ;
	int i = 0;
	for (vector<MosaitCentroid>::const_iterator iCent = sub_centroids.begin() ; iCent < sub_centroids.end() ; iCent++) 
	{
		i++;
		cerr << "\t";
		iCent->printNbImages(prefix+"-\t");
		if (! (i % 5))
			cerr << endl << prefix;
	}
	cerr << endl;
}
void MosaitCentroid::dumpInFile(string & file)
{
	std::ofstream ofile (file.c_str());
	dumpInStream(ofile);
}
void MosaitCentroid::split(int nb_sub_centroids, int max) 
{
	has_sub_centroids = true;
	sub_centroids = vector<MosaitCentroid>(nb_sub_centroids, MosaitCentroid());
	init(images, max);
	images = vector<MosaitImg>();
}

MosaitImg MosaitCentroid::getClosestImage(MosaitImg & image, bool removeit) 
{
	if (has_sub_centroids)
	{
		vector<MosaitImg> vImg;
		vector<MosaitCentroid>::iterator lastbest = sub_centroids.end();
		vector<MosaitCentroid>::iterator best = sub_centroids.begin();		
		float distance, best_distance = 1000;
		for(vector<MosaitCentroid>::iterator iCent = sub_centroids.begin() ; iCent < sub_centroids.end() ; iCent++) {
			if (lastbest == iCent)
				continue;
			distance = iCent->virtual_img.getDistanceWith(image, false);
			if (distance < best_distance) 
			{
				best_distance = distance;
				best = iCent;
			}
		}
		MosaitImg img = best->getClosestImage(image, removeit);
		if (!best->getNbImages()) {
			sub_centroids.erase(best);
		}
		return img;		
	}
	vector<MosaitImg>::iterator best = images.begin() ;
	float distance, best_distance = 1000;
	for(vector<MosaitImg>::iterator iImg = images.begin() ; iImg < images.end() ; iImg++) 
	{
		distance = iImg->getDistanceWith(image);
		if (distance < best_distance) {
//			cerr << "tb: " << iImg->getFileName() << endl;
			best_distance = distance;
			best = iImg;
		}
	}
	MosaitImg result = *best;
	result.setDistanceWith(image);
	if (removeit) {
		images.erase(best);
	}
//	cerr << " t: " << best->getFileName() << " " << best->getDistance() << " " << best_distance << " " <<  best->getDistanceWith(image) << endl;
//	exit(0);
//	sort(images.begin(), images.end(), MosaitImg::compareByDistance);
	return result;
}

bool MosaitCentroid::compareBySize(const MosaitCentroid & a, const MosaitCentroid & b) 
{
	return a.getNbImages() < b.getNbImages();
}

void MosaitCentroid::updateVirtualImage()
{
	virtual_img.setAverage(images);
}

void MosaitCentroid::addImage(MosaitImg & image) 
{
	images.push_back(image);
}

int MosaitCentroid::getNbImages() const
{
	if (!has_sub_centroids)
		return images.size();
	int nb = 0;
	for(int i = 0 ; i < sub_centroids.size() ; i++)
		nb += sub_centroids[i].getNbImages();
	return nb;
}

bool MosaitCentroid::operator==(const MosaitCentroid & centroid) const
{
	if (has_sub_centroids)
		return (sub_centroids == centroid.sub_centroids);
	return images == centroid.images;
}

int MosaitCentroid::getNbCentroids() const
{
	if (! has_sub_centroids)
		return sub_centroids.size();	
	int nb = sub_centroids.size();
	for (int i = 0 ; i < sub_centroids.size() ; i++)
		nb+= sub_centroids[i].getNbCentroids();
	return nb;
}

std::ostream& operator<<(std::ostream& os, MosaitCentroid & mc) 
{
	return mc.dumpInStream(os);
}
std::ostream& MosaitCentroid::dumpInStream(std::ostream& os)
{
	os << sub_centroids.size() << endl;
	for (vector<MosaitCentroid>::iterator iCent = sub_centroids.begin() ; iCent < sub_centroids.end() ; iCent++)
		os << *iCent ;
	os << virtual_img << endl;
	os << images.size() << endl;
	for (vector<MosaitImg>::iterator iImg = images.begin() ; iImg < images.end() ; iImg++) 
		os << *iImg << endl;
	return os;
}

std::istream& operator>>(std::istream& is, MosaitCentroid & mc) 
{
	mc.sub_centroids.clear();
	mc.images.clear();
	int size;
	is >> size;
	mc.has_sub_centroids = (size > 0);
	for (int i = 0 ; i < size  && !is.eof() ; i++ ) {
		MosaitCentroid cent;
		is >> cent;
		mc.sub_centroids.push_back(cent);
	}
	is >> mc.virtual_img;
	is >> size;
	for (int i = 0 ; i < size && !is.eof() ; i++ ) {
		MosaitImg img;
		is >> img;
		mc.images.push_back(img);
	}
	return is;
}
