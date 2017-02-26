#include <iostream>
#include <vector>
#include <math.h>

#include "neuralnetworkloader.h"
#include "neuralnetwork.h"

using namespace std;

//  Scaling layer
vector<double> ScalingLayerCalculateOutputs(const vector<double>& inputs, ScalingLayer scaling_layer)
{
    vector<double> outputs(12);

    for(size_t i = 0; i < 12; ++i)
    {
        if(scaling_layer.statistics[i].maximum-scaling_layer.statistics[i].minimum < 1e-99)
            outputs[i] = inputs[i];
        else
            outputs[i] = 2.0*(inputs[i] - scaling_layer.statistics[i].minimum)/(scaling_layer.statistics[i].maximum - scaling_layer.statistics[i].minimum) - 1.0;
    }

    return(outputs);
}

//  Unscaling layer
vector<double> UnscalingLayerOutputs(const vector<double>& inputs, UnscalingLayer unscaling_layer)
{
    vector<double> outputs(3);

    for(size_t i = 0; i < 3; ++i)
    {
        if(unscaling_layer.statistics[i].maximum - unscaling_layer.statistics[i].minimum < 1e-99)
            outputs[i] = inputs[i];
        else
            outputs[i] = 0.5*(inputs[i] + 1.0)*(unscaling_layer.statistics[i].maximum-unscaling_layer.statistics[i].minimum) + unscaling_layer.statistics[i].minimum;
    }

    return(outputs);
}

//  Activation
double PerceptronCalculateActivation(const double& combination, int activation_function)
{
   switch(activation_function)
   {
      case HyperbolicTangent:
         return(1.0-2.0/(exp(2.0*combination)+1.0));
      break;

      case Linear:
         return(combination);
      break;
   }
}

//  Activations
vector<double> PerceptronLayerCalculateActivations(const vector<double>& combinations, unsigned int current_layer_size, PerceptronLayer perceptron_layer)
{
   const size_t perceptrons_number = perceptron_layer.perceptrons_size;

   vector<double> activations(perceptrons_number);

   for(size_t i = 0; i < perceptrons_number; i++)
      activations[i] = PerceptronCalculateActivation(combinations[i], perceptron_layer.perceptrons[i].activation_function);

   return(activations);
}

//  Combination
double PerceptronCalculateCombination(const vector<double>& inputs, unsigned int previous_layer_size, Perceptron perceptron)
{
   double combination = perceptron.bias;

   for(size_t i = 0; i < previous_layer_size; i++)
       combination += perceptron.synaptic_weights[i]*inputs[i];

   return(combination);
}

//  Combinations
vector<double> PerceptronLayerCalculateCombinations(const vector<double>& inputs, unsigned int current_layer_size, unsigned int previous_layer_size, PerceptronLayer perceptron_layer)
{
   const size_t perceptrons_number = perceptron_layer.perceptrons_size;

   vector<double> combination(perceptrons_number);

   for(size_t i = 0; i < current_layer_size; i++)
      combination[i] = PerceptronCalculateCombination(inputs, previous_layer_size, perceptron_layer.perceptrons[i]);

   return(combination);
}

//  Multilayer perceptron
vector<double> MultilayerPerceptronCalculateOutputs(const vector<double>& inputs, MultilayerPerceptron multilayer_perceptron)
{
    vector<double> outputs;

    unsigned int architecture[] = {12, 20, 10, 3};

    outputs = PerceptronLayerCalculateActivations(PerceptronLayerCalculateCombinations(inputs, architecture[1], architecture[0], multilayer_perceptron.perceptron_layers[0]), architecture[1], multilayer_perceptron.perceptron_layers[0]);

    for(size_t i = 1; i < 3; i++)
        outputs = PerceptronLayerCalculateActivations(PerceptronLayerCalculateCombinations(outputs, architecture[i + 1], architecture[i], multilayer_perceptron.perceptron_layers[i]), architecture[i + 1], multilayer_perceptron.perceptron_layers[i]);

    return(outputs);
}


vector<double> CalculateOutputs(const vector<double>& inputs, NeuralNetwork neural_network)
{
    vector<double> outputs(inputs);

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

    vector<double> inputs;
    for(unsigned int i = 0; i < 12; ++i)
        inputs.push_back(1.0);

    vector<double> outputs = CalculateOutputs(inputs, neural_network);

    for(unsigned int i = 0; i < outputs.size(); ++i)
        cout << outputs[i] << ' ';

    return EXIT_SUCCESS;
}
