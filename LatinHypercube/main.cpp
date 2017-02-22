#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>

#include "pbrtloader.h"

using namespace std;

typedef union Point_s {
    float pos[3];
    struct { float x, y, z; };
} Point;


bool parseFile(string fileName, vector<string> &part, vector<string> &line);
void parseMat(string line, float *vec, unsigned int size);
string changePos(string line, const Point &pos);
string changeMat(string line, const Point &pos);
void samplePos(Point up, Point down, unsigned int sample_number, vector<Point> &res);

int main(int argc, char** argv) {
    if(argc != 4)
    {
        std::cout << "Usage : " << argv[0] << " file nbCameraSample nbLightSample" << endl;
        return 1;
    }

    unsigned int nSampleC = stoi(argv[2]), nSampleL = stoi(argv[3]);
    Point up, down;
    vector<string> part, line;
    vector<Point> sampleC, sampleL;

    //  Extract down and up points
    PBRTLoader loader;
    AABB box;
    string file_string(argv[1]);
    loader.extractAABBFromFile(file_string, box);

    down.pos[0] = box.V[0].x;
    down.pos[1] = box.V[0].y;
    down.pos[2] = box.V[0].z;

    up.pos[0] = box.V[1].x;
    up.pos[1] = box.V[1].y;
    up.pos[2] = box.V[1].z;

    //parseMat(argv[5], up.pos, 3);
    //parseMat(argv[4], down.pos, 3);

    parseFile(argv[1], part, line);

    samplePos(up, down, nSampleC, sampleC);
    samplePos(up, down, nSampleL, sampleL);

    for (unsigned int i = 0; i < nSampleC; ++i) {
        for (unsigned int j = 0; j < nSampleL; ++j) {
            ofstream new_file("scene_" + to_string(i * nSampleC + j) +".pbrt");

            new_file << part[0];
            new_file << changeMat(line[0], sampleC[i]) << std::endl;
            new_file << part[1];
            new_file << changePos(line[1], sampleL[j]) << std::endl;
            new_file << part[2];

            new_file.close();
        }
    }

    return 0;
}

bool parseFile(string fileName, vector<string> &part, vector<string> &line) {
    ifstream file(fileName.c_str());
    string curr_line, curr_part;

    if (file.is_open()) {
        while (getline(file, curr_line)) {
            std::size_t foundC = curr_line.find("Transform");
            std::size_t foundL = curr_line.find("LightSource");

            if (foundC != string::npos || foundL != string::npos) {
                line.push_back(curr_line);
                part.push_back(curr_part);

                curr_part.clear();
            }
            else {
                curr_part += curr_line + '\n';
            }
        }

        part.push_back(curr_part);
        return true;
    }

    return false;
}

void parseMat(string line, float *vec, unsigned int size) {
    size_t d = 2;

    for (unsigned int i = 0; i < size; ++i) {
        line = line.substr(d);
        vec[i] = stof(line, &d);
    }
}

string changePos(string line, const Point &pos) {
    stringstream ss(line);
    string partA, partB;

    size_t f = line.find("from\"");
    size_t f2 = line.substr(f).find("]");

    partA = line.substr(0, f+5);
    partB = line.substr(f + f2 + 1);

    return partA +
            " [ " + to_string(pos.x) + " " + to_string(pos.y) + " " + to_string(pos.z) + " ] "
            + partB;
}

string changeMat(string line, const Point &pos) {
    string new_line;
    size_t f = line.find("[");
    float vec[16];

    parseMat(line.substr(f), vec, 16);

    vec[12] = pos.x; vec[13] = pos.y; vec[14] = pos.z;

    new_line = line.substr(0, f) + " [ ";

    for(int i = 0; i < 16; ++i) {
        new_line += to_string(vec[i]);
        new_line += " ";
    }


    return new_line + " ]";
}

void samplePos(Point up, Point down, unsigned int sample_number, vector<Point> &res) {
    vector<float> sample[3];

    res.clear();

    srand (static_cast <unsigned> (time(0)));

    for (int i = 0; i < 3; ++i) {
        float var = (up.pos[i] - down.pos[i]) / sample_number;

        for (float j = down.pos[i]; j < up.pos[i]; j += var) {
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

            sample[i].push_back(j + r * var);
        }

        random_shuffle(sample[i].begin(), sample[i].end());
    }

    for (unsigned int i = 0; i < sample[0].size(); ++i) {
        Point p;

        for(int j = 0; j < 3; ++j)
            p.pos[j] = sample[j][i];

        res.push_back(p);
    }
}
