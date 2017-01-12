#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "learningmodule.h"

class ConfigParser
{
public:
    ConfigParser();

    bool parseConfig(const std::string &config_file_path, NeuralNetworkArchitecture &neural_network_architecture, LearningParameters &learning_parameter);

private:
    std::vector<std::string> splitValues(const char *values);
    bool isNodeOk(const tinyxml2::XMLNode *pNode);
};

#endif // CONFIGPARSER_H
