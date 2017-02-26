#include <iostream>
#include <vector>
#include <math.h>

#include "neuralnetworkloader.h"
#include "neuralnetwork.h"

using namespace std;

//  Scaling layer
float *ScalingLayerCalculateOutputs(float inputs[MAX_LAYER_SIZE], ScalingLayer scaling_layer)
{
    static float outputs[MAX_LAYER_SIZE];

    for(size_t i = 0; i < 12; ++i)
    {
        outputs[i] = inputs[i];
        if(scaling_layer.statistics[i].maximum-scaling_layer.statistics[i].minimum >= 1e-99)
            outputs[i] = 2.0 * (inputs[i] - scaling_layer.statistics[i].minimum) * (1.0 / (scaling_layer.statistics[i].maximum - scaling_layer.statistics[i].minimum)) - 1.0;
    }

    return(outputs);
}

//  Unscaling layer
float *UnscalingLayerOutputs(float inputs[MAX_LAYER_SIZE], UnscalingLayer unscaling_layer)
{
    static float outputs[MAX_LAYER_SIZE];

    for(size_t i = 0; i < 3; ++i)
    {
        outputs[i] = inputs[i];
        if(unscaling_layer.statistics[i].maximum - unscaling_layer.statistics[i].minimum >= 1e-99)
            outputs[i] = 0.5 * (inputs[i] + 1.0) * (unscaling_layer.statistics[i].maximum-unscaling_layer.statistics[i].minimum) + unscaling_layer.statistics[i].minimum;
    }

    return(outputs);
}

//  Activations
float *PerceptronLayerCalculateActivationsHyperbolicTangent(float combinations[MAX_LAYER_SIZE], unsigned int current_layer_size)
{
   static float activations[MAX_LAYER_SIZE];

   for(size_t i = 0; i < current_layer_size; ++i)
      activations[i] = 1.0 - (2.0 * (1.0 / (exp(2.0 * combinations[i]) + 1.0)));

   return(activations);
}
float *PerceptronLayerCalculateActivationsLinear(float combinations[MAX_LAYER_SIZE], unsigned int current_layer_size)
{
   static float activations[MAX_LAYER_SIZE];

   for(size_t i = 0; i < current_layer_size; ++i)
      activations[i] = combinations[i];

   return(activations);
}

//  Combination
float PerceptronCalculateCombination(float inputs[MAX_LAYER_SIZE], unsigned int previous_layer_size, Perceptron perceptron)
{
   float combination = perceptron.bias;

   for(size_t i = 0; i < previous_layer_size; ++i)
       combination += perceptron.synaptic_weights[i] * inputs[i];

   return(combination);
}

//  Combinations
float *PerceptronLayerCalculateCombinations(float inputs[MAX_LAYER_SIZE], unsigned int current_layer_size, unsigned int previous_layer_size, PerceptronLayer perceptron_layer)
{
   static float combination[MAX_LAYER_SIZE];

   for(size_t i = 0; i < current_layer_size; ++i)
      combination[i] = PerceptronCalculateCombination(inputs, previous_layer_size, perceptron_layer.perceptrons[i]);

   return(combination);
}

//  Multilayer perceptron
float *MultilayerPerceptronCalculateOutputs(float inputs[MAX_LAYER_SIZE], MultilayerPerceptron multilayer_perceptron)
{
    //float outputs[MAX_LAYER_SIZE];
    float *outputs;

    uint architecture[4];
    architecture[0] = 12;
    architecture[1] = 20;
    architecture[2] = 10;
    architecture[3] = 3;

    outputs = PerceptronLayerCalculateActivationsHyperbolicTangent(PerceptronLayerCalculateCombinations(inputs, architecture[1], architecture[0], multilayer_perceptron.perceptron_layers[0]), architecture[1]);
    outputs = PerceptronLayerCalculateActivationsHyperbolicTangent(PerceptronLayerCalculateCombinations(outputs, architecture[2], architecture[1], multilayer_perceptron.perceptron_layers[1]), architecture[2]);
    outputs = PerceptronLayerCalculateActivationsLinear(PerceptronLayerCalculateCombinations(outputs, architecture[3], architecture[2], multilayer_perceptron.perceptron_layers[2]), architecture[3]);

    return(outputs);
}


float *CalculateOutputs(float inputs[MAX_LAYER_SIZE], NeuralNetwork neural_network)
{
    //float outputs[MAX_LAYER_SIZE];
    float *outputs;

    outputs = ScalingLayerCalculateOutputs(inputs, neural_network.scaling_layer);
    outputs = MultilayerPerceptronCalculateOutputs(outputs, neural_network.multilayer_perceptron);
    outputs = UnscalingLayerOutputs(outputs, neural_network.unscaling_layer);

    return(outputs);
}

int main(int argc, char* argv[])
{
    NeuralNetwork neural_network;
    NeuralNetworkLoader neural_network_loader;
    neural_network_loader.loadFile("neuralnetwork.xml", neural_network);

    float inputs[MAX_LAYER_SIZE];
    for(unsigned int i = 0; i < 12; ++i)
        inputs[i] = i + 1;

    float *outputs;

    outputs = CalculateOutputs(inputs, neural_network);

    for(unsigned int i = 0; i < 3; ++i)
        cout << outputs[i] << ' ';

    return EXIT_SUCCESS;
}
