#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>

#include "../tinyxml2/tinyxml2.h"

#include "trainer.h"
#include "configparser.h"

#include <dirent.h>

#ifndef WIN32
    #include <sys/types.h>
#endif

#define EPSILON 0.000001

bool extractArguments(int argc, char *argv[], std::string &path_to_training_folder, std::string &neuralnetwork_file);
bool writeLog(std::string file_name, std::string folder_path, const bool &extraction_ok);
bool isInLog(std::string file_name, std::string folder_path);
bool extractDataSet(const std::string &data_set_path, DataSet &data_set);
bool writeMat(Matrix<double> matrix, std::string folder_path, int nbLines);

/*
 * Input :
 *
 * -Training folder path
 *      This folder will contain training datas (.data) files,
 *      DataSet and NeuralNetwork configuration files (config.xml)
 *      and eventually a NeuralNetwork to be trained (neuralnetworksave[1|2].xml)
 *      and a log file (training.log) which lists already used files
 *
 * Output :
 *
 * -Create and train a NeuralNetwork (eventually from file) with provided datas
 *      Each time a file has been used, the NeuralNetwork will be saved in the oldest file
 *      (neuralnetworksave 1 or 2) and the used fill will be written inside log file
 *
 */
int main(int argc, char *argv[])
{
    std::cout << std::endl;

    std::string path_to_training_folder,
                neural_network_save1_path("neuralnetworksave1.xml"),
                neural_network_save2_path("neuralnetworksave2.xml"),
                neuralnetwork_file;

    DIR *rep = 0;

    // 1- Extract arguments of the program
    if(!extractArguments(argc, argv, path_to_training_folder, neuralnetwork_file))
    {
        std::cerr << "MAIN::ERROR : problem during extraction of arguments" << std::endl;
        return EXIT_FAILURE;
    }

    NeuralNetworkParameters neural_network_parameters;
    DataSetParameters data_set_parameters;
    ConfigParser config_parser;

    // 2- Extract NeuralNetwork and DataSet parameters from the config.xml file
    if(!config_parser.parseConfig(path_to_training_folder + "/config.xml", neural_network_parameters, data_set_parameters))
    {
        std::cerr << "MAIN::ERROR : problem while parsing the config file" << std::endl;
        return EXIT_FAILURE;
    }

    // 3- Process training
    Trainer trainer;
    if(neuralnetwork_file == "")
        trainer.init(neural_network_parameters);
    else
        trainer.init(neuralnetwork_file);

    rep = opendir(path_to_training_folder.c_str());
    struct dirent* file_read = 0;
    bool file1 = false;
    //  Loop over files inside the folder
    while((file_read = readdir(rep)))
    {
        std::string file_name(file_read->d_name);
        size_t point_pos = file_name.find_last_of(".");
        if(point_pos < file_name.size())
        {
            std::string extension = file_name.substr(point_pos);
            if(extension == ".data" && !isInLog(file_name, path_to_training_folder))
            {
                bool extraction_ok = false;
                DataSet data_set;
                if(extractDataSet(path_to_training_folder+"/"+file_name, data_set))
                {
                    extraction_ok = true;
                    trainer.trainNetwork(data_set, data_set_parameters);
                    if(file1)
                        trainer.saveNetwork(neural_network_save1_path);
                    else
                        trainer.saveNetwork(neural_network_save2_path);
                    file1 = !file1;
                }
                if(!writeLog(file_name,path_to_training_folder, extraction_ok))
                    std::cerr << "MAIN::ERROR : A problem occured when trying to write in training.log" << std::endl;
            }
        }
    }

    trainer.saveNetwork("neuralnetwork.xml");   //  Training finished, save file

    if (closedir(rep) == -1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}


/*
 * Check if arguments are correct
 */
bool extractArguments(int argc, char *argv[], std::string &path_to_training_folder, std::string &neuralnetwork_file)
{
    if(argc != 2)
    {
        std::cerr << "MAIN::ERROR : incorrect number of arguments" << std::endl
                  << "use : ./LearningModule <path_to_training_folder>" << std::endl;

        return false;
    }

    path_to_training_folder = argv[1];

    struct dirent* file_read = 0;
    bool    config_file_found = false,
            log_file_found = false,
            xml1_file_found = false,
            xml2_file_found = false;

    DIR *rep = 0;

    //  Open folder
    std::cout << "Learning folder : " << path_to_training_folder << std::endl;
    rep = opendir(path_to_training_folder.c_str());
    if(!rep)
    {
        std::cerr << "MAIN::ERROR : input folder could not be opened" << std::endl;
        return false;
    }

    //  List every file
    while((file_read = readdir(rep)) != 0)
    {
        std::string file_name(file_read->d_name);
        size_t point_pos = file_name.find_last_of(".");
        if(point_pos < file_name.size())
        {
            if(!config_file_found && file_name == "config.xml")
                config_file_found = true;
            else if(!log_file_found && file_name == "training.log")
                log_file_found = true;
            else if(!xml1_file_found && file_name == "neuralnetworksave1.xml")
                xml1_file_found = true;
            else if(!xml2_file_found && file_name == "neuralnetworksave2.xml")
                xml2_file_found = true;
        }
    }

    if(!config_file_found)
    {
        std::cerr << "MAIN::EXTRACTARGUMENTS::ERROR : config file not found in " << path_to_training_folder << std::endl
                  << "config file must be named config.xml" << std::endl;

        return false;
    }
    else
        std::cout << "- config.xml FOUND" << std::endl;


    if(log_file_found)
        std::cout << "- training.log FOUND" << std::endl;
    else
        std::cout << "- training.log NOT FOUND" << std::endl;

    neuralnetwork_file = "";

    if(xml1_file_found)
    {
        std::cout << "- neuralnetworksave1.xml FOUND" << std::endl;
        neuralnetwork_file = "neuralnetworksave1.xml";
    }
    else
        std::cout << "- neuralnetworksave1.xml NOT FOUND" << std::endl;


    if(xml2_file_found)
    {
        std::cout << "- neuralnetworksave2.xml FOUND" << std::endl;
        neuralnetwork_file = "neuralnetworksave2.xml";
    }
    else
        std::cout << "- neuralnetworksave2.xml NOT FOUND" << std::endl;

    if(xml1_file_found && xml2_file_found)
    {
        std::cerr << "MAIN::EXTRACTARGUMENTS::ERROR both neuralnetworksave1.xml and neuralnetworksave2.xml are present, abort" << std::endl;
        return false;
    }

    if (closedir(rep) == -1)
        return EXIT_FAILURE;

    return true;
}

/*
 * Write the input file_name in training.log located in folder_path
 */
bool writeLog(std::string file_name, std::string folder_path, const bool &extraction_ok)
{
    std::ofstream log_file(folder_path + "/training.log", std::ios::app);

    if ( !log_file.is_open() )
      return false;
    else
    {
        log_file << file_name;
        if(!extraction_ok)
            log_file << " - ERROR";
        log_file << std::endl;
        return true;
    }
}

/*
 * Write matrice to a file named "matrix.txt" for debugging
 */
bool writeMat(Matrix<double> matrix, std::string folder_path, int nbLines)
{
    std::ofstream mat_file(folder_path + "/matrix.txt", std::ios::trunc);

    if ( !mat_file.is_open() )
    {
        std::cout<<"error while opening matrix.txt"<<std::endl;
        return false;
    }
    else
    {
        //printing matrix in a file
        for (unsigned int line = 0; line < nbLines; ++line) {
            for (unsigned int col = 0; col < 15; ++col) {
                mat_file << matrix(line,col) << " ";
            }
            mat_file << std::endl;
        }
        return true;
    }
}

/*
 * Check if the input file_name is already in training.log located in folder_path
 */
bool isInLog(std::string file_name, std::string folder_path)
{
    std::ifstream log_file(folder_path + "/training.log");
    std::string line;

    while (std::getline(log_file, line))
        if(line == file_name)
            return true;

    return false;
}


struct vec3
{
    double x;
    double y;
    double z;
};


/*
 * Create a DataSet from a .data file
 *j
 * The stucture of the file must respect the following structure:
 *
 * <camera_position.x> <camera_position.y> <camera_position.z>
 * <light_position.x> <light_position.y> <light_position.z>
 * <fragment_position.x> <fragment_position.y> <fragment_position.z> <normal.x> <normal.y> <normal.z> <R> <G> <B>  * N (N = number of datas)
 * */
bool extractDataSet(const std::string &data_set_path, DataSet &data_set)
{
    FILE* data_file;

    data_file = fopen(data_set_path.c_str(),"rb");

    if(!(data_file))
    {
        std::cerr << "MAIN::EXTRACTDATASET::ERROR Problem while opening " << data_set_path << std::endl;
        return false;
    }

    unsigned int prodRes;
    float   camera_position[3],
            light_position[3],
            fragment_position[3],
            normal[3],
            color[3];

    //  Read fixed datas
    fread(&prodRes, sizeof(int), 1, data_file);
    fread(&camera_position, sizeof(float), 3, data_file);
    fread(&light_position, sizeof(float), 3, data_file);

    //  Read 1st instance
    fread(fragment_position, sizeof(float), 3, data_file);
    fread(normal, sizeof(float), 3, data_file);
    fread(color, sizeof(float), 3, data_file);

    //  Set 1st row of matrix, matrix needs to have at least 1 row if we want to append
    Matrix<double> data_matrix(1, 15);
    data_matrix(0, 0) = fragment_position[0]; data_matrix(0, 1) = fragment_position[1]; data_matrix(0, 2) = fragment_position[2];
    data_matrix(0, 3) = camera_position[0];   data_matrix(0, 4) = camera_position[1];   data_matrix(0, 5) = camera_position[2];
    data_matrix(0, 6) = light_position[0];    data_matrix(0, 7) = light_position[1];    data_matrix(0, 8) = light_position[2];
    data_matrix(0, 9) = normal[0];            data_matrix(0, 10) = normal[1];           data_matrix(0, 11) = normal[2];
    data_matrix(0, 12) = color[0];            data_matrix(0, 13) = color[1];            data_matrix(0, 14) = color[2];

    //  Loop over other instances
    for(unsigned int i = 1; i < prodRes; ++i)
    {
        //  Read instance
        fread(fragment_position, sizeof(float), 3, data_file);
        fread(normal, sizeof(float), 3, data_file);
        fread(color, sizeof(float), 3, data_file);

        //  Fill instance vector
        Vector<double> instance(15);
        instance[0] = fragment_position[0]; instance[1] = fragment_position[1]; instance[2] = fragment_position[2];
        instance[3] = camera_position[0];   instance[4] = camera_position[1];   instance[5] = camera_position[2];
        instance[6] = light_position[0];    instance[7] = light_position[1];    instance[8] = light_position[2];
        instance[9] = normal[0];            instance[10] = normal[1];           instance[11] = normal[2];
        instance[12] = color[0];            instance[13] = color[1];            instance[14] = color[2];

        //  Check if instance is not already in Matrix
        bool instance_found = false;
        unsigned int j = 0;
        while(j < data_matrix.get_rows_number())    //  Row loop
        {
            bool row_equal = true;
            unsigned int k = 0;
            while(k < 15)
            {
                if(fabs((data_matrix(j, k) - instance[k])) > EPSILON)    //  Column loop
                {
                    row_equal = false;
                    break;
                }
                ++k;
            }
            if(row_equal)
            {
                instance_found = true;
                break;
            }
            ++j;
        }

        if(!instance_found)
            data_matrix.append_row(instance);
    }
    //writeMat(data_matrix, "./", data_matrix.get_rows_number());

    if(data_matrix.get_rows_number() <= 1)  //  Only one line left in DataSet
        return false;

    data_set.set_data(data_matrix);

    fclose(data_file);

    return true;
}
