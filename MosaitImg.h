/**
 * Mosait by Tangui Morlier
 * Under GPLv3
 */
#ifndef MOSAIT_IMG
#define MOSAIT_IMG
#include "MosaitCommon.h"
#include "MosaitPixel.h"
#include <vector>
#include <jerror.h>

#ifdef _MSC_VER
  #ifdef BUILDING_DLL
    #define DLLEXPORT __declspec(dllexport)
  #else
    #define DLLEXPORT __declspec(dllimport)
  #endif
  #define DLLLOCAL
#else
  #ifdef HAVE_GCCVISIBILITYPATCH
    #define DLLEXPORT __attribute__ ((visibility("default")))
    #define DLLLOCAL __attribute__ ((visibility("hidden")))
  #else
    #define DLLEXPORT
    #define DLLLOCAL
  #endif
#endif

extern "C" DLLEXPORT void function(int a);

class DLLEXPORT MosaitImg
{
	public:
		MosaitImg(string & file);
		MosaitImg(fipImage & image) ;
//		MosaitImg(fipImage::fipImage image) ;
		MosaitImg(int size_x, int size_y);
		MosaitImg();
		string getFileName();
		inline float getDistance() const {
			return distance;
		}
/*		inline bool equal(const MosaitImg& i) const{
			return operator==(i);
		}
*/
		inline bool operator==(const MosaitImg& i) const{
//			cout << "operator== on Img\n";
			return i.pixels == pixels;
		};

		inline bool operator<(const MosaitImg& i ) const {
			cerr << "operator< on Img\n";
			return i.pixels < pixels;
		};
		friend std::ostream& operator<<(std::ostream& os, MosaitImg mi);
		friend istream & operator >>(istream &ins, MosaitImg & mi);
		static bool compareByDistance(const MosaitImg &i1, const MosaitImg &i2);
		float getDistanceWith(const MosaitImg & img, bool withlight = 1) const;
		void  setDistanceWith(const MosaitImg & img, bool withlight = 1);
		void setAverage(const vector<MosaitImg> images);
		inline void incrUsed() { nb_used ++ ; };
		inline int getNbUsed() const {return nb_used;};
	private:
		string filename;
		vector<MosaitPixel> pixels;
		float distance;
		void init(fipImage & image) ;
		int nb_used;
};


#endif
