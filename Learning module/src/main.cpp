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

bool extractArguments(int argc, char *argv[], std::string &path_to_training_folder, std::string &neuralnetwork_file);
bool writeLog(std::string file_name, std::string folder_path);
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

    //NeuralNetwork neural_network;
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
                DataSet data_set;
                extractDataSet(path_to_training_folder+"/"+file_name, data_set);
                trainer.trainNetwork(data_set, data_set_parameters);
                if(file1)
                    trainer.saveNetwork(neural_network_save1_path);
                else
                    trainer.saveNetwork(neural_network_save2_path);
                file1 = !file1;
                std::cout << "LOG WRITE" << std::endl;
                if(!writeLog(file_name,path_to_training_folder))
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
bool writeLog(std::string file_name, std::string folder_path)
{
    std::ofstream log_file(folder_path + "/training.log", std::ios::app);

    if ( !log_file.is_open() )
      return false;
    else
    {
        log_file << file_name << std::endl;
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
        for (int line = 0; line < nbLines; ++line) {
            for (int col = 0; col < 15; ++col) {
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
 *
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

    int prodRes;
    float camera_position[3];
    float light_position[3];
    float fragment_position[3];
    float normal[3];
    float color[3];

    fread(&prodRes, sizeof(int), 1, data_file);

    Matrix<double> data_matrix(prodRes, 15);

    fread(&camera_position, sizeof(float), 3, data_file);
    fread(&light_position, sizeof(float), 3, data_file);

    for (unsigned int i = 0; i < prodRes; ++i) {
        fread(fragment_position, sizeof(float), 3, data_file);
        fread(normal, sizeof(float), 3, data_file);
        fread(color, sizeof(float), 3, data_file);

        data_matrix(i, 0) = fragment_position[0]; data_matrix(i, 1) = fragment_position[1]; data_matrix(i, 2) = fragment_position[2];
        data_matrix(i, 3) = camera_position[0]; data_matrix(i, 4) = camera_position[1]; data_matrix(i, 5) = camera_position[2];
        data_matrix(i, 6) = light_position[0]; data_matrix(i, 7) = light_position[1]; data_matrix(i, 8) = light_position[2];
        data_matrix(i, 9) = normal[0]; data_matrix(i, 10) = normal[1]; data_matrix(i, 11) = normal[2];
        data_matrix(i, 12) = color[0]; data_matrix(i, 13) = color[1]; data_matrix(i, 14) = color[2];
    }
    writeMat(data_matrix, "./", prodRes);
    data_set.set_data(data_matrix);

    fclose(data_file);

    return true;
}
