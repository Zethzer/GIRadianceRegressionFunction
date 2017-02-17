#ifndef NEURALNETWORKLOADER_H
#define NEURALNETWORKLOADER_H

#include <string>
#include <vector>
#include "tinyxml2.h"
#include "neuralnetwork.h"

/*
 * Load a NeuralNetwork from a XML file
 * The .xml must follow the structure of
 * OpenNN neural networks
 * */
class NeuralNetworkLoader
{
public:
    NeuralNetworkLoader();

    void loadFile(const std::string &file_name, NeuralNetwork &neural_network);

private:
    void ScalingLayerFromXML(const tinyxml2::XMLDocument& document, ScalingLayer &scaling_layer);
    void MultilayerPerceptronFromXML(const tinyxml2::XMLDocument& document, MultilayerPerceptron &multilayer_perceptron);
    void UnscalingLayerFromXML(const tinyxml2::XMLDocument& document, UnscalingLayer &unscaling_layer);
    bool errorHandler(const char *msg) const;
    std::vector<std::string> splitValues(const char *values) const;
};

#endif // NEURALNETWORKLOADER_H
