#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "../tinyxml2/tinyxml2.h"

#include "trainer.h"
#include "configparser.h"

#include <dirent.h>

#ifndef WIN32
    #include <sys/types.h>
#endif

bool extractArguments(int argc, char *argv[], std::string &path_to_training_folder);
bool writeLog(std::string file_name, std::string folder_path);
bool isInLog(std::string file_name, std::string folder_path);

/*
 * Inputs :
 *
 * -Training folder path
 *
 * Output :
 *
 * Create a NeuralNetwork, train it with
 * DataSet and save it to the defined path,
 * or if not defined, to the default path
 */
int main(int argc, char *argv[])
{
    std::string path_to_training_folder,
                neural_network_save1_path("neuralnetworksave1.xml"),
                neural_network_save2_path("neuralnetworksave2.xml");

    // 1- Extract arguments of the program
    if(!extractArguments(argc, argv, path_to_training_folder))
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
    //Trainer trainer(neural_network_parameters);
    //trainer.trainNetwork(training_data_set_path, data_set_parameters);
    //trainer.saveNetwork(neural_network_save_path);

/////////////
    DIR* rep = 0;
    struct dirent* file_read = 0;
    bool file1 = true;

    rep = opendir(path_to_training_folder.c_str());

    if (rep == 0)
        return EXIT_FAILURE;

    while ((file_read = readdir(rep)) != 0)
    {
        std::string file_name(file_read->d_name);
        size_t point_pos = file_name.find_last_of(".");
        if(point_pos < file_name.size())
        {
            std::string extension = file_name.substr(point_pos);
            if(extension == ".data")
            {
                if(!isInLog(file_name, path_to_training_folder))
                {
                    //DataSet data_set = extractDataSet(file_name);
                    //trainer.trainNetwork(data_set, data_set_parameters); // TODO ROMOU Penser à modifier

                    if(file1)
                    {
                        //trainer.saveNetwork(neural_network_save1_path);
                        std::cout << "Trained " << file_name << " in " << neural_network_save1_path << std::endl;
                        file1 = !file1;
                    }
                    else
                    {
                        //trainer.saveNetwork(neural_network_save2_path);
                        std::cout << "Trained " << file_name << " in " << neural_network_save2_path << std::endl;
                        file1 = !file1;
                    }
                    if(!writeLog(file_name,path_to_training_folder))
                        std::cerr << "MAIN::ERROR : A problem occured when trying to write in training.log" << std::endl;
                }
                else
                {
                    std::cout << file_name << " already in training.log - Nothing to do..." << std::endl;
                }
            }
        }
    }

    if (closedir(rep) == -1)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
///////////////


/*
 * Check if arguments are correct
 */
bool extractArguments(int argc, char *argv[], std::string &path_to_training_folder)
{
    if(argc != 2)
    {
        std::cerr << "MAIN::ERROR : incorrect number of arguments" << std::endl
                  << "use : ./LearningModule <path_to_training_folder>" << std::endl;

        return false;
    }

    path_to_training_folder = argv[1];

    DIR* rep = 0;
    struct dirent* file_read = 0;
    bool    config_file_found = false,
            log_file_found = false,
            xml1_file_found = false,
            xml2_file_found = false;

    rep = opendir(path_to_training_folder.c_str());

    if (rep == 0)
    {
        std::cerr << "MAIN::ERROR : path_to_training_folder could not be opened" << std::endl;
        return false;
    }

    while ((file_read = readdir(rep)) != 0)
    {
        std::string file_name(file_read->d_name);
        size_t point_pos = file_name.find_last_of(".");
        if(point_pos < file_name.size())
        {
            std::string extension = file_name.substr(point_pos);
            if(file_name == "config.xml")
                config_file_found = true;
            else if(file_name == "training.log")
                log_file_found = true;
            else if(file_name == "neuralnetworksave1.xml")
                xml1_file_found = true;
            else if(file_name == "neuralnetworksave2.xml")
                xml2_file_found = true;
        }
    }

    if(!config_file_found)
    {
        std::cerr << "MAIN::ERROR : config file not found in " << path_to_training_folder << std::endl
                  << "config file must be named config.xml" << std::endl;

        return false;
    }

    if(!log_file_found)
    {
        //std::ofstream log_file(path_to_training_folder + "/training.log");
    }

    if (closedir(rep) == -1)
        return false;

    return true;
}

/*
 * Write the input file_name in training.log located in folder_path
 */
bool writeLog(std::string file_name, std::string folder_path)
{
    std::ofstream log_file(folder_path + "/training.log", std::ios::app);

    if ( !log_file.is_open() ) {
      return false;
    }
    else {
        log_file << file_name;
        log_file << std::endl;
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
    {
        if(line == file_name)
            return true;
    }
    return false;
}
