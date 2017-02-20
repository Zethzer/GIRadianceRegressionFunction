#include <iostream>
#include <vector>

#include "../tinyxml2/tinyxml2.h"

#include "trainer.h"
#include "configparser.h"

#include "../boost/filesystem.hpp"

bool extractArguments(int argc, char *argv[], std::string &training_data_set_path, std::string &neural_network_save_path);

/*
 * Inputs :
 *
 * -DataSet path (.dat file)
 * -NeuralNetwork save path (.xml file)
 *
 * Output :
 *
 * Create a NeuralNetwork, train it with
 * DataSet and save it to the defined path,
 * or if not defined, to the default path
 */
int main(int argc, char *argv[])
{
    /*std::string training_data_set_path,
                neural_network_save_path;

    // 1- Extract arguments of the program
    if(!extractArguments(argc, argv, training_data_set_path, neural_network_save_path))
    {
        std::cerr << "MAIN::ERROR : problem during extraction of arguments" << std::endl;
        return EXIT_FAILURE;
    }


    NeuralNetworkParameters neural_network_parameters;
    DataSetParameters data_set_parameters;
    ConfigParser config_parser;

    // 2- Extract NeuralNetwork and DataSet parameters from the config.xml file
    if(!config_parser.parseConfig("config.xml", neural_network_parameters, data_set_parameters))
    {
        std::cerr << "MAIN::ERROR : problem while parsing the config file" << std::endl;
        return EXIT_FAILURE;
    }

    // 3- Process training
    Trainer trainer(neural_network_parameters);
    trainer.trainNetwork(training_data_set_path, data_set_parameters);
    trainer.saveNetwork(neural_network_save_path);

    return EXIT_SUCCESS;*/

    for(boost::filesystem::directory_iterator itr(path_ss); itr!=directory_iterator(); ++itr)
    {
        std::cout << itr->path().filename() << ' '; // display filename only
        if (boost::filesystem::is_regular_file(itr->status())) std::cout << " [" << boost::filesystem::file_size(itr->path()) << ']';
        std::cout << '\n';
    }
}



/*
 * Check if arguments are correct
 */
bool extractArguments(int argc, char *argv[], std::string &training_data_set_path, std::string &neural_network_save_path)
{
    if(argc < 3)
    {
        std::cerr << "MAIN::ERROR : incorrect number of arguments" << std::endl
                  << "use : ./LearningModule <data_set_file_path> <neural_network_save_path>" << std::endl;

        return false;
    }

    training_data_set_path = argv[1];

    std::string data_set_format = training_data_set_path.substr(training_data_set_path.size() - 4, 4);
    if(data_set_format != ".dat")
    {
        std::cerr << "MAIN::ERROR : incorrect dataset file format" << std::endl
                  << "dataset file must be .dat file" << std::endl;

        return false;
    }

    neural_network_save_path = argv[2];
    std::string neural_network_format = neural_network_save_path.substr(neural_network_save_path.size() - 4, 4);
    if(neural_network_format != ".xml")
    {
        std::cerr << "MAIN::ERROR : incorrect neural network save file format" << std::endl
                  << "neural network save file must be .xml file" << std::endl;

        return false;
    }

    return true;
}
