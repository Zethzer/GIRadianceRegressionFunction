#include <iostream>
#include <vector>

#include "../tinyxml2/tinyxml2.h"

#include "learningmodule.h"
#include "configparser.h"

bool extractArguments(int argc, char *argv[], std::string &training_data_set_path, std::string neural_network_save_path);

/*
 * Inputs :
 *
 * - DataSet path, .dat file
 * - NeuralNetwork save path .xml file
 *
 * Output :
 *
 * Create a NeuralNetwork, train it with
 * DataSet and save it to the defined path,
 * or if not defined, to the default path
 */
int main(int argc, char *argv[])
{
    std::string training_data_set_path,
                neural_network_save_path;

    if(!extractArguments(argc, argv, training_data_set_path, neural_network_save_path))
        return EXIT_FAILURE;

    NeuralNetworkArchitecture neural_network_architecture;
    LearningParameters learning_parameters;
    ConfigParser config_parser;
    config_parser.parseConfig("config.xml", neural_network_architecture, learning_parameters);


    LearningModule learning_module(neural_network_architecture, learning_parameters);
    learning_module.trainNetwork(training_data_set_path);
    learning_module.saveNetwork(neural_network_save_path);

    return EXIT_SUCCESS;
}

/*
 * Check if arguments are correct
 * set optional argument to default if not set
 * */
bool extractArguments(int argc, char *argv[], std::string &training_data_set_path, std::string neural_network_save_path)
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
