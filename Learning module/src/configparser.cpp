#include "configparser.h"

using namespace tinyxml2;

ConfigParser::ConfigParser()
{

}

/*
 * Parse the 'config.xml' file
 * containing architecture of the network
 * and parameters of the learning
 * */
bool ConfigParser::parseConfig(const std::string &config_file_path, NeuralNetworkArchitecture &neural_network_architecture, LearningParameters &learning_parameter)
{
    XMLDocument config_file;

    bool load_ok = config_file.LoadFile(config_file_path.c_str());

    if(!load_ok)
    {
        std::cerr << "EXTRACTCONFIGURATION::ERROR failed to load xml config file" << std::endl;
        return false;
    }

    XMLNode *pRoot;
    XMLElement *pNetwork;
    XMLElement *pElement;
    XMLElement *pLearning;

    pRoot = config_file.FirstChild();

    if(!isNodeOk(pRoot))
        return false;

    pNetwork = pRoot->FirstChildElement("Network");
    if(!isNodeOk(pNetwork))
        return false;

    pElement = pNetwork->FirstChildElement("Architecture");
    if(!isNodeOk(pElement))
        return false;
    const char *architecture_char = pElement->GetText();

    pElement = pNetwork->FirstChildElement("InputsNames");
    if(!isNodeOk(pElement))
        return false;
    const char *inputs_names_char = pElement->GetText();

    pElement = pNetwork->FirstChildElement("OutputsNames");
    if(!isNodeOk(pElement))
        return false;
    const char *outputs_names_char = pElement->GetText();

    pElement = pNetwork->FirstChildElement("ScalingMethod");
    if(!isNodeOk(pElement))
        return false;
    const char *scaling_method_char = pElement->GetText();

    pElement = pNetwork->FirstChildElement("UnscalingMethod");
    if(!isNodeOk(pElement))
        return false;
    const char *unscaling_method_char = pElement->GetText();

    pLearning = pRoot->FirstChildElement("Learning");
    if(!isNodeOk(pLearning))
        return false;

    pElement = pLearning->FirstChildElement("Method");
    if(!isNodeOk(pElement))
        return false;
    const char *method_char = pElement->GetText();

    pElement = pLearning->FirstChildElement("MaxError");
    if(!isNodeOk(pElement))
        return false;
    const char *max_error_char = pElement->GetText();

    std::vector<std::string> architecture_strings = splitValues(architecture_char);
    std::vector<std::string> inputs_names_strings = splitValues(inputs_names_char);
    std::vector<std::string> outputs_names_strings = splitValues(outputs_names_char);
    std::string scaling_method_string = scaling_method_char;
    std::string unscaling_method_string = unscaling_method_char;

    Vector<size_t> network_architecture;
    for(size_t i = 0; i < architecture_strings.size(); ++i)
    {
        size_t element;
        sscanf(architecture_strings[i].c_str(), "%zu", &element);
        network_architecture.push_back(element);
    }

    Vector<std::string> variables_names;
    for(size_t i = 0; i < inputs_names_strings.size(); ++i)
        variables_names.push_back(inputs_names_strings[i]);
    for(size_t i = 0; i < outputs_names_strings.size(); ++i)
        variables_names.push_back(outputs_names_strings[i]);

    ScalingLayer::ScalingMethod scaling_method;
    if(scaling_method_string == "NoScaling")
        scaling_method = ScalingLayer::ScalingMethod::NoScaling;
    else if(scaling_method_string == "MinimumMaximum")
        scaling_method = ScalingLayer::ScalingMethod::MinimumMaximum;
    else if(scaling_method_string == "MeanStandardDeviation")
        scaling_method = ScalingLayer::ScalingMethod::MeanStandardDeviation;

    UnscalingLayer::UnscalingMethod unscaling_method;
    if(unscaling_method_string == "NoUnscaling")
        unscaling_method = UnscalingLayer::UnscalingMethod::NoUnscaling;
    else if(unscaling_method_string == "MinimumMaximum")
        unscaling_method = UnscalingLayer::UnscalingMethod::MinimumMaximum;
    else if(unscaling_method_string == "MeanStandardDeviation")
        unscaling_method = UnscalingLayer::UnscalingMethod::MeanStandardDeviation;

     neural_network_architecture.init(network_architecture, variables_names, scaling_method, unscaling_method);

     std::string method_string = method_char;
     double max_error = atof(max_error_char);

     TrainingMethod training_method;
     if(method_string == "QuasiNewton")
         training_method = TrainingMethod::QuasiNewton;
     else if(method_string == "LevenbergMarquardt")
         training_method = TrainingMethod::LevenbergMarquardt;

     learning_parameter.init(training_method, max_error);

     return true;
}

/*
 * Split a char* of values separated by ' '
 * return a string vector of the values
 * */
std::vector<std::string> ConfigParser::splitValues(const char *values)
{
    std::vector<std::string> out;

    std::string values_string = values;
    char delimiter = ' ';

    size_t pos = 0;
    std::string token;
    while((pos = values_string.find(delimiter)) != std::string::npos)
    {
        token = values_string.substr(0, pos);
        out.push_back(token);
        values_string.erase(0, pos + 1);
    }

    return out;
}

/*
 * Check if XMLNode is not null
 * */
bool ConfigParser::isNodeOk(const XMLNode *pNode)
{
    if(pNode == nullptr)
    {
        std::cerr << "EXTRACTCONFIGURATION::ERROR problem while parsing xml document" << std::endl;
        return false;
    }
    return true;
}
