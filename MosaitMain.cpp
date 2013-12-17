/**
 * Mosait by Tangui Morlier
 * Under GPLv3
 */
#include "MosaitImg.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include "MosaitCentroid.h"
#include <sys/stat.h>

using namespace std;

/*function... might want it in some class?*/
int getdir (string & dir, vector<string> &files)
{
	DIR *dp;
	if (dir[dir.length() -1] != '/')
		dir += '/';
	struct dirent *dirp;
	if((dp = opendir(dir.c_str())) == NULL) {
		cerr << "Error(" << errno << ") opening " << dir << endl;
		return errno;
	}

	while ((dirp = readdir(dp)) != NULL) {
		files.push_back(dir + string(dirp->d_name));
	}
	closedir(dp);
	return 0;
}

int getin(vector<string>&files)
{
	string file;
	while(!cin.eof())
	{
		cin >> file;
		files.push_back(file);
	}
}
int load_images(vector<MosaitImg> & images, vector<string> & files) 
{
	int i = 0;
	for (vector<string>::iterator iFile = files.begin() 
	; iFile < files.end() //&& i < 100
	; iFile++) 
	{
		if (iFile->find("jpg") == string::npos)
			continue;
		try {
			MosaitImg image (*iFile);
			images.push_back(image);
		}catch( ... ){
			cerr << "Error while loading image " << *iFile << endl;
			continue;
		}
		i++;
		if (i % 1000 == 0)
			cerr << i << endl;
	}
}

int load_centroid_from_file(string & file, MosaitCentroid & centroid)
{
	ifstream myfile;
	myfile.open(file.c_str());
	myfile >> centroid;
	cerr << centroid.getNbImages() << " images and " << centroid.getNbCentroids() << " centroids loaded " << endl;
} 

int load_centroid_from_in(MosaitCentroid & centroid)
{
	cin >> centroid;
	cerr << centroid.getNbImages() << " images and " << centroid.getNbCentroids() << " centroids loaded " << endl;
}

int load_from_in(vector<pair<int, MosaitImg> > & images, vector<vector<MosaitImg> > & bestimages)
{
	int centri_size = 0;
	int images_size = 0;
	cin >> centri_size;
	bestimages = vector<vector<MosaitImg> > (centri_size);
	cerr << "centri_size " << bestimages.size() << endl;
	for(int i = 0 ; i < centri_size ; i++) {
		MosaitImg image_ref;
		cin >> image_ref;
		images.push_back(pair<int, MosaitImg>(i, image_ref));
		cin >> images_size;
		for (int y = 0 ; y < images_size ; y++)
		{
			MosaitImg image;
			cin >> image;
			bestimages[i].push_back(image);
		}
	}
} 

int save_to_file(MosaitCentroid & centroid, string & outfile)
{
	std::ofstream file (outfile.c_str());
	file << centroid << endl;
}

int save_to_out(MosaitCentroid & centroid)
{
	cout << centroid << endl;
}

int save_to_out(vector<MosaitImg> & images)
{
	cout << 1 << endl;
	cout << MosaitImg(DEF_IMG_X, DEF_IMG_Y) << endl;
	cout << images.size() << endl;
	for (vector<MosaitImg>::iterator i = images.begin() ; i < images.end() ; i++) {
		cout << *i << endl;
	}
}

bool compareByDistance(const pair<int, MosaitImg> & ir1, const pair<int, MosaitImg> & ir2)
{
	return MosaitImg::compareByDistance(ir1.second, ir2.second);
}

void mosait(fipImage::fipImage & image, MosaitCentroid & centroid, vector<MosaitImg> & images_dest, bool uniq = true)
{
	cerr << "Crop images..." << endl;
	vector<pair<int, MosaitImg> > images_src;
	int i = 0;
	for(int y = 0 ; y < image.getHeight()/DEF_IMG_Y -1; y++) {
		for (int x = 0 ; x < image.getWidth()/DEF_IMG_X -1; x++) {
			fipImage::fipImage small;
			image.copySubImage(small, x*DEF_IMG_X, y*DEF_IMG_Y, x*DEF_IMG_X+DEF_IMG_X, y*DEF_IMG_Y+DEF_IMG_Y);
			images_src.push_back(pair<int, MosaitImg> (i, small));
//			cerr << images_src.back().second << endl;
			i++;
		}
	}
	cerr << "Mosait with " << centroid.getNbImages() << " images and " << centroid.getNbCentroids() << " centroids !!!! " << endl;
	cerr << "Original image cut into " << images_src.size() << " pieces" << endl;
	if (centroid.getNbImages() < images_src.size()) {
		cerr << "Not enougth images in the db to compute unique algorithm. Duplicated forced" << endl;
		uniq = false;
	}
	time_t debut = time(NULL);
	images_dest = vector<MosaitImg> (images_src.size());
	random_shuffle(images_src.begin(), images_src.end());
	cerr << "|" ;
	int current = 0;
	int size = images_src.size();
	int scale = 50;
	int cpt = scale;
	cerr << "0%";
	cpt -= 2;
	for ( ;cpt>0; cpt--)
		cerr << " " ;
	cerr << "|100%" << endl << "|";
	for(vector<pair<int, MosaitImg > >::iterator iSrc = images_src.begin() ; iSrc < images_src.end() ; iSrc++) 
	{
		cpt++;
		images_dest[iSrc->first] = centroid.getClosestImage(iSrc->second, uniq);
		if (cpt*scale/size > current) {
			cerr << "=" ;
			current = cpt*scale/size;
		}
	}
	cerr << "|" << endl;

	float sum = 0;
	int nb = 0;
	for (vector<MosaitImg>::iterator iDest = images_dest.begin() ; iDest < images_dest.end() ; iDest++)
	{
		sum += iDest->getDistance();
		nb++;
	}
	cout << time(NULL) - debut << " ";
	cout << sum/nb << endl;
}

void print_images(vector<MosaitImg> & images_dest, fipImage::fipImage & image_orig, ostream & screen) 
{
	int x = 0;
	screen << endl;
	for(vector<MosaitImg>::iterator iImg = images_dest.begin(); iImg < images_dest.end() ; iImg++) 
	{
		screen << iImg->getFileName() << endl;
		x++;
		if (x >=  image_orig.getWidth()/DEF_IMG_X -1) {
			screen << endl;
			x = 0;
		}
	}
}

void save_final_image(vector<MosaitImg> & images_dest, fipImage::fipImage & image_orig, string final_name, int max_final_size = 0) 
{
	cerr << "Create final image..." << endl;
	WORD left = 0;
	WORD top  = 0;
	fipImage::fipImage img;
        if (!img.load(images_dest[0].getFileName().c_str()))
		cerr << "cannot load " << images_dest[0].getFileName() << endl;
	WORD max_left = img.getWidth() *(image_orig.getWidth()/DEF_IMG_X -1);
	WORD max_top =  img.getHeight()*(image_orig.getHeight()/DEF_IMG_Y -1);
	fipImage::fipImage finalImage;
	finalImage.setSize (FIT_BITMAP, max_left, max_top, 24);
	cerr << "final image: "<< max_left << "x" << max_top << endl;
	for (vector<MosaitImg>::iterator iDest = images_dest.begin() ; iDest < images_dest.end() ; iDest++)
	{
		string file = iDest->getFileName();
		if (!img.load(file.c_str()))
			cerr << "cannot load " << file << endl;
		finalImage.pasteSubImage(img, left, top);
		left += img.getWidth();
		if (left  >= max_left) {
			left = 0;
			top += img.getHeight();
		}
		
	}
	if (max_final_size) {
		if(!finalImage.makeThumbnail(max_final_size))
			cerr << "thumbnail error\n";
	}
	if(!finalImage.save(final_name.c_str()))
		cerr << "saving error "<<  final_name << " (probably wrong image extension)" << endl;
	else
		cerr << final_name << " saved\n";
}

bool less_size(const vector<MosaitImg> & vi1, const vector<MosaitImg> & vi2)
{
	return vi1.size() > vi2.size();
}

MosaitCentroid clusterit(int nb_centroids, MosaitCentroid & mc1, int nb) {
	MosaitCentroid mc (mc1, nb_centroids, nb);
	return mc;
}
MosaitCentroid clusterit(int nb_centroids, MosaitCentroid & mc1, int nb, string & out) {
        MosaitCentroid mc (mc1, nb_centroids, nb, out);
        return mc;
}

void help() {
	cout << "Usage:\n";
	cout << "\tCreate image db\n\t\tmosait -c <images_dir> -o <output_db_file>\n";
	cout << "\tOptimize image db\n\t\tmosait -O <nb_centroids> -d <db_file> [-o <output_db_file>] [-n <max_images_per_centroid>]\n";
	cout << "\tCreate mosaic\n\t\tmosait -d <db_file> -i <src_image> -o <output_image> [-s <max_size_output_image>] [-u] [-t <output_txt>]\n";
	cout << "\tInteractive mode\n\t\tmosait -I\n";
}

void get_images_from_file(istream & iF , vector<string> & files)
{
	string file;
	char cfile [255];
	while (!iF.eof()) {
		iF.getline(cfile, 255);
		files.push_back(cfile);
	}
}

void create_db(char *&  imgs, char *&  db)
{
      string simgs (imgs);
      string sdb(db);
      vector<string> files;
      struct stat results;
      if (stat(imgs, &results)) {
	cerr << "Cannot access to " << simgs << endl;
	return;
      }
      if ((results.st_mode & S_IFMT) == S_IFDIR) {
	cerr << "Create db from directory\n";
	getdir(simgs,files);
      } else {
	ifstream iF(simgs.c_str());
	cerr << "Create db from file \n";
	get_images_from_file(iF, files);
      } 
      vector<MosaitImg> images;
      load_images(images, files);
      MosaitCentroid centroid(images);
      save_to_file(centroid, sdb);
}


void optimize_db(char *&  db, int nb_centroid, int max_imgs, char *& out_db)
{
	string sdb(db);
	string sout_db(out_db);
	if (!out_db)
		sout_db = sdb;
	MosaitCentroid centroid;
        load_centroid_from_file(sdb, centroid);
	if (!sout_db.length())
	        centroid = clusterit(nb_centroid, centroid, max_imgs);
	else
                centroid = clusterit(nb_centroid, centroid, max_imgs, sout_db);
        save_to_file(centroid, sout_db);
}

void mosait_do(char* & db, char* & in_img, char* & out_img, bool uniq, int size, char* & out_txt)
{
        MosaitCentroid centroid;
	string sdb(db);
	load_centroid_from_file(sdb, centroid);
	fipImage::fipImage image_orig;
        image_orig.load(in_img);
	vector<MosaitImg> final_images;
        mosait(image_orig, centroid, final_images, uniq);
        ofstream myfile;
	if (out_txt) {
	        myfile.open(out_txt);
                print_images(final_images, image_orig, myfile);
                myfile.close();
	}
        save_final_image(final_images, image_orig, out_img, size);
}

int interactive()
{
	try {
		string cmd;
		cerr << "You are using mosait in interactive mode" <<endl;
		cerr << "The following commands are available :\n\tdb <file> : the file containing images info to load as db" << endl;
		cerr << "\tprint <file> <is_uniq> <fileout> : print into <fileout>" << endl;
		cerr << "\tcreate <file> <is_uniq> <fileout> <size> <filetxt>" << endl;
		MosaitCentroid init_centroid;
		while (!cin.eof()) {
			string cmd;
			cin >> cmd;
			bool uniq;
			if (cmd == "print") 
			{
				MosaitCentroid centroid = init_centroid;
				string file;
				cin >> file;
				cin >> uniq;
				cerr << cmd << " " << file << " " << uniq << " ";
				string fileout;
				cin >> fileout;
				cerr << fileout << endl;
				vector<MosaitImg> final_images;
				fipImage::fipImage image_orig;
				image_orig.load(file.c_str());
				mosait(image_orig, centroid, final_images, uniq);
				ofstream myfile;
				myfile.open(fileout.c_str());
				print_images(final_images, image_orig, myfile);
				myfile.close();
			}else if (cmd == "db") {
				string file;
				cin >> file;
				cerr << cmd << " " << file << endl;
				load_centroid_from_file(file, init_centroid);
			}else if (cmd == "create") {
				MosaitCentroid centroid = init_centroid;
				string file;
				cin >> file;
				cin >> uniq;
				string fileout;
				cin >> fileout;
				int size = 1000;
				cin >> size ;
				string filetxt;
				cin >> filetxt;
				cerr << cmd << " " << file << " " << uniq << " " << fileout << " " << size << " " << filetxt << endl;
				vector<MosaitImg> final_images;
				try {
					fipImage::fipImage image_orig;
					image_orig.load(file.c_str());
					mosait(image_orig, centroid, final_images, uniq);
					ofstream myfile;
                	                myfile.open(filetxt.c_str());
                        	        print_images(final_images, image_orig, myfile);
                                	myfile.close();
					save_final_image(final_images, image_orig, fileout, size);
					cerr << fileout << " done\n";
				}catch(...) {
					cerr << "error" << endl;
				}
			}else {
				cin.clear();
			}
			sleep(1);
		}
	}catch(int e) {
		cout << "Exception " <<  e << endl;
	}
	return 0;
}

int main(int argc, char** argv)
{
        int c;
        char *cdir = NULL;
        char *cdb = NULL;
        char *out_res = NULL;
        char *in_img = NULL;
        char *out_txt = NULL;
        bool uniq = true;
        int nb_centroids = 0;
	int max_imgs = 0;
        bool interact = false;
        int size = 0;
        while ((c = getopt (argc, argv, "c:d:s:uo:i:t:O:m:Ih")) != -1)
         switch (c)
          {
                case 'c':
                        cdir = optarg;
                        break;
                case 'd':
                        cdb = optarg;
                        break;
                case 's':
                        size = atoi(optarg);
                        break;
                case 'u':
                        uniq = false;
                        break;
                case 'o':
                        out_res = optarg;
                        break;
                case 'i':
                        in_img = optarg;
                        break;
                case 't':
                        out_txt = optarg;
                        break;
                case 'O':
                        nb_centroids = atoi(optarg);
                        break;
		case 'm':
			max_imgs = atoi(optarg);
			break;
                case 'I':
                        interactive();
                        exit(0);
		case 'h':
			break;
                default:
                        help();
                        exit(1);
        }
	if (cdir && out_res) {
		create_db(cdir, out_res);
		exit(0);
	}
	if (nb_centroids && cdb) {
		optimize_db(cdb, nb_centroids, max_imgs, out_res);
		exit(0);
	}
	if (cdb) {
		mosait_do(cdb, in_img, out_res, uniq, size, out_txt); 
		exit(0);
	}
	help();
}


