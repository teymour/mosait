/**
 * Mosait by Tangui Morlier
 * Under GPLv3
 */
#include "MosaitImg.h"
#include "MosaitPixel.h"
#include <FreeImagePlus.h>
#include <vector>

MosaitImg::MosaitImg() : filename()
{}

MosaitImg::MosaitImg(string & file) : filename(file)
{
	fipImage image;
	if (!image.load(file.c_str()))
		throw string("Error while loading image");
	init(image);
}

MosaitImg::MosaitImg(fipImage & image) : filename()
{
	init(image);
}

MosaitImg::MosaitImg(int size_x, int size_y) :filename("randomized")
{
	float r, g, b;
	for(int x = 0; x < DEF_IMG_X ; x++)
	for(int y = 0; y < DEF_IMG_Y ; y++)
	{
		r = int(rand() % 255);
		g = int(rand() % 255);
		b = int(rand() % 255);
		MosaitPixel pixel(r/255.0, g/255.0, b/255.0);
		pixels.push_back(pixel);
	}	
}

void MosaitImg::init(fipImage & image)
{
	fipImage small = image;
	small.rescale(DEF_IMG_X, DEF_IMG_Y, FILTER_BICUBIC);
	small.convertToRGBF();
        FIRGBF *px = (FIRGBF*) small.accessPixels();
	int i = 0;
	for(int x = 0; x < small.getWidth() ; x++)
	    for(int y = 0 ; y < small.getHeight() ; y++) {
		MosaitPixel pixel(px[i].red,px[i].green,px[i].blue);
		pixels.push_back(pixel);
//		cerr << pixels.back() << " " << r << " " << g << " " << b << endl;
		i++;
	}
}

string MosaitImg::getFileName()
{
	return filename;
}

/*float MosaitImg::getDistance() const
{
	return distance;
}
*/



float MosaitImg::getDistanceWith(const MosaitImg & img, bool withlight) const
{
	float res = 0;
	int cpt;
	for(cpt = 0; cpt < pixels.size() ; cpt++ ) 
	{
		res += pixels[cpt].getDistanceWith(img.pixels[cpt], withlight);
	}
	return res/cpt;
}
void  MosaitImg::setDistanceWith(const MosaitImg & img, bool withlight)
{
	distance = getDistanceWith(img, withlight);
}

bool MosaitImg::compareByDistance(const MosaitImg & img1, const MosaitImg & img2)
{
//	cerr << img2.distance << " < " <<  img2.distance << endl;
	return (img1.distance < img2.distance);
}
void MosaitImg::setAverage(const vector<MosaitImg> images) 
{
	if (!images.size())
		return;
	vector<MosaitPixel> uniq_pixels;
	for(int i = 0 ; i < pixels.size() ; i++) 
	{
		uniq_pixels.clear();
		for(int y = 0 ; y < images.size(); y++)
		{
			uniq_pixels.push_back(images[y].pixels[i]);
		}
		pixels[i].setAverage(uniq_pixels);
	}
}
std::ostream& operator<<(std::ostream& os, MosaitImg im) 
{
	os << im.getFileName() << " " << im.pixels.size() << " " ;
	for (vector<MosaitPixel>::iterator iPixel = im.pixels.begin() ; iPixel < im.pixels.end() ; iPixel++)
		os << *iPixel << " ";
	return os;
}


istream & operator >>(istream &ins, MosaitImg & mi) {
	int size = 0;
	ins >> mi.filename >> size;
	mi.pixels = vector<MosaitPixel>();
	for (int i = 0 ; i < size ; i++) 
	{
		MosaitPixel mp;
		ins >> mp;
		mi.pixels.push_back(mp);
	}
}

