/**
 * Mosait by Tangui Morlier
 * Under GPLv3
 */
#ifndef MOSAIT_PIXEL
#define MOSAIT_PIXEL
#include "MosaitCommon.h"
#include <vector>
class MosaitPixel
{
	public:
		MosaitPixel(float r, float g, float b);
		inline bool operator==(const MosaitPixel p) const {
//			cerr << p.rgb_r << "==" << rgb_r << endl;
			return ((p.rgb_r == rgb_r) && (p.rgb_g == rgb_g) && (p.rgb_b == rgb_b));
		};
		inline bool operator<(const MosaitPixel& p ) const {
			cerr << "operator< on Img\n";
			return p.rgb_r < rgb_r;
		};
		friend std::ostream& operator<<(std::ostream& os, MosaitPixel mp);
		friend istream & operator >>(istream &ins, MosaitPixel & mp);
		MosaitPixel();
		float getDistanceWith(const MosaitPixel & pixel, bool withlight = 1) const;
		void setAverage(vector<MosaitPixel> pixels);
	private:
		void fill_xyy();
		float xyY_x;
		float xyY_y;
		float xyY_Y;
		float rgb_r;
		float rgb_g;
		float rgb_b;

};


#endif
