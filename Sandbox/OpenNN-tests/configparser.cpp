#include "configparser.h"

using namespace tinyxml2;

ConfigParser::ConfigParser()
{

}

/*
 * Parse the 'config.xml' file
 * containing architecture of the network
 * and parameters of the learning
 *
 * Returns true if no problem was encountered,
 * false otherwise
 * */
bool ConfigParser::parseConfig(const std::string &config_file_path, NeuralNetworkParameters &neural_network_parameters, DataSetParameters &data_set_parameters)
{
    XMLDocument config_file;

    bool load_ok = config_file.LoadFile(config_file_path.c_str());

    if(load_ok)
    {
        std::cerr << "CONFIGPARSER::ERROR failed to load xml config file" << std::endl;
        return false;
    }

    XMLNode     *p_root;
    XMLElement  *p_network,
                *p_data_set;

    p_root = config_file.FirstChild();
    checkNode(p_root);

    p_network = p_root->FirstChildElement("Network");
    checkNode(p_network);

    p_data_set = p_root->FirstChildElement("DataSet");
    checkNode(p_data_set);

    try
    {
        parseNetwork(p_network, neural_network_parameters);
        parseDataSet(p_data_set, data_set_parameters);
    }
    catch(std::exception &e)
    {
        return false;
    }

    return true;
}

/********************************
 *                              *
 *      NEURAL NETWORK PART     *
 *                              *
 * ******************************/

/*
 * Parse informations related to
 * NeuralNetworkParameters
 * in the config file
 * */
void ConfigParser::parseNetwork(XMLElement *p_network, NeuralNetworkParameters &neural_network_parameters)
{
    Vector<size_t> network_architecture = parseNeuralNetworkArchitecture(p_network);
    Vector<ActivationFunction> layer_activation_functions = parseLayersActivationFunctions(p_network);
    Vector<std::string> variables_names = parseVariablesNames(p_network);
    ScalingMethod scaling_layer_method = parseScalingLayerMethod(p_network);
    UnscalingMethod unscaling_layer_method = parseUnscalingLayerMethod(p_network);

    neural_network_parameters.init(network_architecture, layer_activation_functions, variables_names, scaling_layer_method, unscaling_layer_method);
}

Vector<size_t> ConfigParser::parseNeuralNetworkArchitecture(XMLElement *p_network)
{
    const char *architecture_char = getNodeText(p_network, "Architecture");

    std::vector<std::string> architecture_strings = splitValues(architecture_char);

    Vector<size_t> network_architecture;
    for(size_t i = 0; i < architecture_strings.size(); ++i)
    {
        size_t element;
        sscanf(architecture_strings[i].c_str(), "%zu", &element);
        network_architecture.push_back(element);

        std::cout << element << ' ';
    }

    return network_architecture;
}

Vector<ActivationFunction> ConfigParser::parseLayersActivationFunctions(XMLElement *p_network)
{
    const char *layers_activation_functions_char = getNodeText(p_network, "LayersActivationFunctions");

    std::vector<std::string> layers_activation_functions_strings = splitValues(layers_activation_functions_char);

    Vector<ActivationFunction> layer_activation_functions;
    for(size_t i = 0; i < layers_activation_functions_strings.size(); ++i)
    {
        if(layers_activation_functions_strings[i] == "Threshold")
            layer_activation_functions.push_back(ActivationFunction::Threshold);
        else if(layers_activation_functions_strings[i] == "SymmetricThreshold")
            layer_activation_functions.push_back(ActivationFunction::SymmetricThreshold);
        else if(layers_activation_functions_strings[i] == "Logistic")
            layer_activation_functions.push_back(ActivationFunction::Logistic);
        else if(layers_activation_functions_strings[i] == "HyperbolicTangent")
            layer_activation_functions.push_back(ActivationFunction::HyperbolicTangent);
        else if(layers_activation_functions_strings[i] == "Linear")
            layer_activation_functions.push_back(ActivationFunction::Linear);
        else
        {
            std::cerr << "CONFIGPARSER::ERROR LayersActivationFunctions are not valid" << std::endl;
            throw new std::runtime_error("");
        }
    }

    return layer_activation_functions;
}

Vector<std::string> ConfigParser::parseVariablesNames(XMLElement *p_network)
{
    const char  *inputs_names_char = getNodeText(p_network, "InputsNames"),
                *outputs_names_char = getNodeText(p_network, "OutputsNames");

    std::vector<std::string> inputs_names_strings = splitValues(inputs_names_char);
    std::vector<std::string> outputs_names_strings = splitValues(outputs_names_char);

    Vector<std::string> variables_names;
    for(size_t i = 0; i < inputs_names_strings.size(); ++i)
        variables_names.push_back(inputs_names_strings[i]);
    for(size_t i = 0; i < outputs_names_strings.size(); ++i)
        variables_names.push_back(outputs_names_strings[i]);

    return variables_names;
}

ScalingMethod ConfigParser::parseScalingLayerMethod(XMLElement *p_network)
{
    const char *scaling_layer_method_char = getNodeText(p_network, "ScalingLayerMethod");

    std::string scaling_layer_method_string = scaling_layer_method_char;

    ScalingMethod scaling_layer_method;
    if(scaling_layer_method_string == "NoScaling")
        scaling_layer_method = ScalingMethod::NoScaling;
    else if(scaling_layer_method_string == "MinimumMaximum")
        scaling_layer_method = ScalingMethod::MinimumMaximum;
    else if(scaling_layer_method_string == "MeanStandardDeviation")
        scaling_layer_method = ScalingMethod::MeanStandardDeviation;
    else
    {
        std::cerr << "CONFIGPARSER::ERROR ScalingLayerMethod is not valid" << std::endl;
        throw new std::runtime_error("");
    }

    return scaling_layer_method;
}

UnscalingMethod ConfigParser::parseUnscalingLayerMethod(XMLElement *p_network)
{
    const char *unscaling_layer_method_char = getNodeText(p_network, "UnscalingLayerMethod");


    std::string unscaling_layer_method_string = unscaling_layer_method_char;

    UnscalingMethod unscaling_layer_method;
    if(unscaling_layer_method_string == "NoUnscaling")
        unscaling_layer_method = UnscalingMethod::NoUnscaling;
    else if(unscaling_layer_method_string == "MinimumMaximum")
        unscaling_layer_method = UnscalingMethod::MinimumMaximum;
    else if(unscaling_layer_method_string == "MeanStandardDeviation")
        unscaling_layer_method = UnscalingMethod::MeanStandardDeviation;
    else
    {
        std::cerr << "CONFIGPARSER::ERROR UnscalingLayerMethod is not valid" << std::endl;
        throw new std::runtime_error("");
    }

    return unscaling_layer_method;
}

/********************************
 *                              *
 *         DATA SET PART        *
 *                              *
 * ******************************/

/*
 * Parse informations related to
 * DataSetParameters
 * */
void ConfigParser::parseDataSet(XMLElement *p_data_set, DataSetParameters &data_set_parameters)
{
    XMLElement *p_splitting = p_data_set->FirstChildElement("Splitting");
    checkNode(p_splitting);

    SplittingParameters splitting_parameters = parseSplitting(p_splitting);


    XMLElement *p_training = p_data_set->FirstChildElement("Training");
    checkNode(p_training);

    TrainingParameters training_parameters = parseTraining(p_training);

    data_set_parameters.init(splitting_parameters, training_parameters);
}

SplittingParameters ConfigParser::parseSplitting(XMLElement *p_splitting)
{
    const char *splitting_method_char = getNodeText(p_splitting, "SplittingMethod");
    const char *selection_ratio_char = getNodeText(p_splitting, "SelectionRatio");
    const char *testing_ratio_char = getNodeText(p_splitting, "TestingRatio");

    std::string splitting_method_string = splitting_method_char;

    SplittingMethod splitting_method;
    if(splitting_method_string == "Random")
        splitting_method = SplittingMethod::Random;
    else if(splitting_method_string == "Sequential")
        splitting_method = SplittingMethod::Sequential;
    else
    {
        std::cerr << "CONFIGPARSER::ERROR SplittingMethod is not valid" << std::endl;
        throw new std::runtime_error("");
    }

    double  selection_ratio = atof(selection_ratio_char),
            testing_ratio = atof(testing_ratio_char),
            training_ratio = 1.0 - selection_ratio - testing_ratio;

    return SplittingParameters(splitting_method, training_ratio, selection_ratio, testing_ratio);
}

TrainingParameters ConfigParser::parseTraining(XMLElement *p_training)
{
    const char *scaling_method_char = getNodeText(p_training, "ScalingMethod");
    const char *training_method_char = getNodeText(p_training, "TrainingMethod");
    const char *gradient_norm_goal_char = getNodeText(p_training, "GradientNormGoal");

    std::string scaling_method_string = scaling_method_char;

    DataScalingMethod scaling_method;
    if(scaling_method_string == "MinimumMaximum")
        scaling_method = DataScalingMethod::MinimumMaximum;
    else if(scaling_method_string == "MeanStandardDeviation")
        scaling_method = DataScalingMethod::MeanStandardDeviation;
    else
    {
        std::cerr << "CONFIGPARSER::ERROR ScalingMethod is not valid" << std::endl;
        throw new std::runtime_error("");
    }

    std::string training_method_string = training_method_char;

    TrainingMethod training_method;
    if(training_method_string == "QuasiNewton")
        training_method = TrainingMethod::QUASI_NEWTON_METHOD;
    else if(training_method_string == "LevenbergMarquardt")
        training_method = TrainingMethod::LEVENBERG_MARQUARDT_ALGORITHM;
    else if(training_method_string == "GradientDescent")
        training_method = TrainingMethod::GRADIENT_DESCENT;
    else if(training_method_string == "ConjugateDescent")
        training_method = TrainingMethod::CONJUGATE_GRADIENT;
    else if(training_method_string == "NewtonMethod")
        training_method = TrainingMethod::NEWTON_METHOD;
    else
    {
        std::cerr << "CONFIGPARSER::ERROR TrainingMethod is not valid" << std::endl;
        throw new std::runtime_error("");
    }

    double gradient_norm_goal = atof(gradient_norm_goal_char);


    return TrainingParameters(scaling_method, training_method, gradient_norm_goal);
}


/********************************
 *                              *
 *          UTILITIES           *
 *                              *
 * ******************************/

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
    out.push_back(values_string);

    return out;
}

/*
 * Check if XMLNode is not null
 * */
void ConfigParser::checkNode(const XMLNode *pNode)
{
    if(pNode == nullptr)
    {
        std::cerr << "CONFIGPARSER::ERROR problem while parsing XML file" << std::endl;
        throw new std::runtime_error("");
    }
}

const char *ConfigParser::getNodeText(XMLElement *p_node, const char *node_name)
{
    XMLElement *p_element = p_node->FirstChildElement(node_name);
    if(p_element == nullptr)
    {
        std::cerr << "CONFIGPARSER::ERROR problem while parsing node " << node_name << std::endl;
        throw new std::runtime_error("");
    }

    return p_element->GetText();
}
