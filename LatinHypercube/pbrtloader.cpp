#include "pbrtloader.h"
#include <iostream>
#include <fstream>
#include <strstream>
#include <sstream>

PBRTLoader::PBRTLoader()
{
}

AABB PBRTLoader::extractAABBFromFile(const std::string &pbrt_path, AABB &box)
{
    AABB out_box;

    float x,
          y,
          z;

    // File
    std::ifstream pbrt_file;

    pbrt_file.open(pbrt_path.c_str());

    if(!(pbrt_file))
    {
        std::cerr << "PBRTLOADER::ADDMODELS::ERROR Problem while parsing .pbrt file" << std::endl;
        exit(1);
    }

    std::string word,
                line;

    // Parse file until WorldBegin
    std::getline(pbrt_file, line);
    std::istringstream iss(line);
    iss >> word;
    while(word == "WorldBegin")
    {
        std::getline(pbrt_file, line);
        iss.str(line);
        iss >> word;
    }

    // WorldBegin loop
    while(std::getline(pbrt_file, line))
    {
        iss.clear();
        iss.str(line);
        iss >> word;

        //  Geometry line
        if(word == "Shape")
        {

            bool hasValues = false;
            std::string remaining = line, param, values;
            int pos = remaining.find_first_of("\"");

            // Look for "point P"
            while(pos != std::string::npos)
            {
                param = extractNextParameter(remaining, values, &hasValues, &pos);
                if (param == "point P")
                {
                    std::stringstream ss(values);   //  Clip positions to box
                    while (ss >> x >> y >> z)
                        box.clipPoint(vec3(x, y, z));
                }
            }
        }
    }

    return out_box;
}

std::string PBRTLoader::extractNextParameter(std::string &line, std::string &valuesString, bool *values, int *pos)
{
    std::string temp = line.substr(line.find_first_of("\"") + 1);
    line = temp.substr(temp.find_first_of("\"") + 1);
    if (line.at(1) == '[') {
        *values = true;
        valuesString = extractValues(line);
    }
    int position = line.find_first_of("\"");
    *pos = position;
    if(position != std::string::npos)
        line = line.substr(position);
    temp = temp.substr(0, temp.find_first_of("\""));
    return temp;
}

std::string PBRTLoader::extractValues(std::string &line)
{
    std::string temp = line.substr(line.find_first_of("[") + 1);
    temp = temp.substr(0, temp.find_first_of("]"));
    return temp;
}
