/**
 * Mosait by Tangui Morlier
 * Under GPLv3
 */
#include "MosaitPixel.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

MosaitPixel::MosaitPixel() : rgb_r(0), rgb_g(0), rgb_b(0) {fill_xyy();}

void MosaitPixel::fill_xyy()
{
	float tmpx = rgb_r * 0.576700  + rgb_g * 0.185556  + rgb_b * 0.188212;
	float tmpy = rgb_r * 0.297361  + rgb_g * 0.627355  + rgb_b * 0.0752847;
	xyY_Y = rgb_r * 0.0270328 + rgb_g * 0.0706879 + rgb_b * 0.991248 + 0.00000000000001;
	xyY_x = tmpx / (tmpx + tmpy + xyY_Y) ;
	xyY_y = tmpy / (tmpx + tmpy + xyY_Y) ;
}

MosaitPixel::MosaitPixel(float r, float g, float b)
{	
//	cerr << "rgb: " << r << "," << g << "," << b << endl;
	rgb_r = r;
	rgb_g = g;
	rgb_b = b;
	fill_xyy();
//	cerr << "xyY: " << xyY_x << "," << xyY_y << "," << xyY_Y << endl;
}



float MosaitPixel::getDistanceWith(const MosaitPixel & pixel, bool withlight) const
{

/*	float sum1 = xyY_x + xyY_y + xyY_Y + rgb_r + rgb_g + rgb_b;
	float sum2 = pixel.xyY_x + pixel.xyY_y + pixel.xyY_Y + pixel.rgb_r + pixel.rgb_g + pixel.rgb_b;
	float sum1Sq = pow(xyY_x,2) + pow(xyY_y,2) + pow(xyY_Y,2) +  pow(rgb_r,2) +  pow(rgb_g,2) +  pow(rgb_b,2);
	float sum2Sq = pow(pixel.xyY_x,2) + pow(pixel.xyY_y,2) + pow(pixel.xyY_Y,2) +  pow(pixel.rgb_r,2) +  pow(pixel.rgb_g,2) +  pow(pixel.rgb_b,2);
	float pSum = xyY_x*pixel.xyY_x + xyY_y*pixel.xyY_y + xyY_Y*pixel.xyY_Y+ pixel.rgb_r*rgb_r + pixel.rgb_g*rgb_g + pixel.rgb_b*rgb_b;
	float num = pSum - (sum1*sum2/3);
	float den = sqrt((sum1Sq-pow(sum1,2)/3)*(sum2Sq-pow(sum2,2)/3));
	if (!den || isnan(den))
		return 0;
	return -num/den;
//*/
//	float score = sqrt(pow(xyY_x - pixel.xyY_x, 2) + pow(xyY_y - pixel.xyY_y, 2) + pow(xyY_Y - pixel.xyY_Y, 2) );
	float color =  ( pow(pixel.xyY_x - xyY_x, 2) + pow(pixel.xyY_y - xyY_y, 2) + pow(pixel.rgb_r - rgb_r, 2) + pow(pixel.rgb_g - rgb_g, 2) + pow(pixel.rgb_b - rgb_b, 2) ) ;
//	cerr << pixel.xyY_Y << " " << xyY_Y << " " << light << " " << color << endl;
//	float score = sqrt( (1-pow(2*light-1,2)) * color+light );
//	return score;
//	    $distance[$i] = sqrt( ( - (2*$light-1)**2 + 1 )				  * $color				  + $light );;
	if (!withlight)
		return sqrt(color);
	float light = pow(pixel.xyY_Y - xyY_Y, 2);
	return sqrt(light+color);

}
//*/
void MosaitPixel::setAverage(vector<MosaitPixel> pixels) {
	rgb_r = 0;
	rgb_g = 0;
	rgb_b = 0;
	int nb = 0;
	for (vector<MosaitPixel>::iterator iPix = pixels.begin() ; iPix < pixels.end(); iPix++) {
		nb++;
		rgb_r += iPix->rgb_r;
		rgb_g += iPix->rgb_g;
		rgb_b += iPix->rgb_b;
	}
	rgb_r /= nb;
	rgb_g /= nb;
	rgb_b /= nb;
	fill_xyy();
}

std::ostream& operator<<(std::ostream& os, MosaitPixel mp) 
{
	int r = int(mp.rgb_r*255);
	int g = int(mp.rgb_g*255);
	int b = int(mp.rgb_b*255);
	os << r << " " << g << " " << b ;
	return os;
}

istream & operator >>(istream &ins, MosaitPixel & mp) {
	string str_a, str_b, str_c;
	ins >> str_a >> str_b >> str_c ;
	mp.rgb_r = atof(str_a.c_str())/255;
	mp.rgb_g = atof(str_b.c_str())/255;
	mp.rgb_b = atof(str_c.c_str())/255;
	mp.fill_xyy();
	return ins;
}
