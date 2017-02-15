#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

typedef union Point_s {
    float pos[3];
    struct { float x, y, z; };
} Point;


bool parseFile(string fileName, vector<string> &part, vector<string> &line);
Point parsePos(string line);
string changePos(string line, const Point &pos);
void samplePos(Point up, Point down, unsigned int sample_number, vector<Point> &res);

int main(int argc, char** argv) {
    if(argc != 6) {
        std::cout << "Usage : " << argv[0] << " file nbCameraSample nbLightSample upPoint downPoint" << endl;
        return 1;
    }

    unsigned int nSampleC = stoi(argv[2]), nSampleL = stoi(argv[3]);
    Point up = parsePos(argv[4]), down = parsePos(argv[5]);
    vector<string> part, line;
    vector<Point> sampleC, sampleL;

    parseFile(argv[1], part, line);

    samplePos(up, down, nSampleC, sampleC);
    samplePos(up, down, nSampleL, sampleL);

    for (unsigned int i = 0; i < nSampleC; ++i) {
        for (unsigned int j = 0; j < nSampleL; ++j) {
            ofstream new_file(/*nom du file*/);

            new_file << part[0];
            new_file << changePos(line[0], sampleC[i]);
            new_file << part[1];
            new_file << changePos(line[1], sampleL[j]);
            new_file << part[2];

        }
    }

    return 0;
}

bool parseFile(string fileName, vector<string> &part, vector<string> &line) {
    ifstream file(fileName.c_str());
    string curr_line, curr_part;

    if (file.is_open()) {
        while (getline(file, curr_line)) {
            std::size_t foundC = curr_line.find("Camera");
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

Point parsePos(string line) {
    Point point;
    size_t d = 2;

    for (unsigned int i = 0; i < 3; ++i) {
        line = line.substr(d);
        point.pos[i] = stof(line, &d);
    }

    return point;
}

string changePos(string line, const Point &pos) {
    stringstream ss(line);
    string partA, partB;

    size_t f = line.find("from\"");
    size_t f2 = line.substr(f).find("]");

    partA = line.substr(0, f+5);
    partB = line.substr(f + f2 + 1);

    return partA +
            " [" + to_string(pos.x) + " " + to_string(pos.y) + " " + to_string(pos.z) + " ] "
            + partB;
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
