#include <iostream>

#include "neuralnetworkloader.h"
#include "neuralnetwork.h"


int main(int argc, char* argv[])
{
    NeuralNetwork neural_network;
    NeuralNetworkLoader neural_network_loader;
    neural_network_loader.loadFile("neuralnetwork.xml", neural_network);



    return EXIT_SUCCESS;
}

Vector<double> NeuralNetwork::calculate_outputs(const Vector<double>& inputs) const
{
    Vector<double> outputs(inputs);

    outputs = scaling_layer_pointer->calculate_outputs(inputs);
    outputs = multilayer_perceptron_pointer->calculate_outputs(outputs);
    outputs = unscaling_layer_pointer->calculate_outputs(outputs);

    return(outputs);
}

//  Scaling layer
Vector<double> ScalingLayerCalculateOutputs(const Vector<double>& inputs) const
{
    const size_t scaling_neurons_number = get_scaling_neurons_number();

    Vector<double> outputs(scaling_neurons_number);

    for(size_t i = 0; i < scaling_neurons_number; i++)
    {
        if(statistics[i].maximum-statistics[i].minimum < 1e-99)
            outputs[i] = inputs[i];
        else
            outputs[i] = 2.0*(inputs[i] - statistics[i].minimum)/(statistics[i].maximum-statistics[i].minimum) - 1.0;
    }

    return(outputs);
}

//  Multilayer perceptron
Vector<double> MultilayerPerceptron::calculate_outputs(const Vector<double>& inputs) const
{
    const size_t layers_number = get_layers_number();

    Vector<double> outputs;

    if(layers_number == 0)
    {
        return(outputs);
    }
    else
    {
        outputs = layers[0].calculate_outputs(inputs);

        for(size_t i = 1; i < layers_number; i++)
        {
            outputs = layers[i].calculate_outputs(outputs);
        }
    }

    return(outputs);
}

//  Activation
double Perceptron::calculate_activation(const double& combination) const
{
   switch(activation_function)
   {
      case Perceptron::HyperbolicTangent:
      {
         return(1.0-2.0/(exp(2.0*combination)+1.0));
      }
      break;

      case Perceptron::Threshold:
      {
         if(combination < 0)
         {
            return(0.0);
         }
         else
         {
            return(1.0);
         }
      }
      break;

      case Perceptron::Linear:
      {
         return(combination);
      }
      break;

      default:
      break;
   }
}

//  Activations
Vector<double> PerceptronLayer::calculate_activations(const Vector<double>& combinations) const
{
   const size_t perceptrons_number = get_perceptrons_number();

   // Calculate activation from layer

   Vector<double> activations(perceptrons_number);

   for(size_t i = 0; i < perceptrons_number; i++)
      activations[i] = perceptrons[i].calculate_activation(combinations[i]);

   return(activations);
}

//  Combination
double Perceptron::calculate_combination(const Vector<double>& inputs) const
{
    const size_t inputs_number = get_inputs_number();

   // Calculate combination

   double combination = bias;

   for(size_t i = 0; i < inputs_number; i++)
   {
       combination += synaptic_weights[i]*inputs[i];
   }

   return(combination);
}

//  Combinations
Vector<double> PerceptronLayer::calculate_combinations(const Vector<double>& inputs) const
{
   const size_t perceptrons_number = get_perceptrons_number();

   // Calculate combination to layer

   Vector<double> combination(perceptrons_number);

   for(size_t i = 0; i < perceptrons_number; i++)
   {
      combination[i] = perceptrons[i].calculate_combination(inputs);
   }

   return(combination);
}

//  Layer
Vector<double> PerceptronLayer::calculate_outputs(const Vector<double>& inputs) const
{
   return(calculate_activations(calculate_combinations(inputs)));
}

//  Unscaling layer
Vector<double> UnscalingLayerOutputs(const Vector<double>& inputs) const
{
    const size_t unscaling_neurons_number = get_unscaling_neurons_number();

    Vector<double> outputs(unscaling_neurons_number);

    for(size_t i = 0; i < unscaling_neurons_number; i++)
    {
        if(statistics[i].maximum - statistics[i].minimum < 1e-99)
            outputs[i] = inputs[i];
        else
            outputs[i] = 0.5*(inputs[i] + 1.0)*(statistics[i].maximum-statistics[i].minimum) + statistics[i].minimum;
    }

    return(outputs);
}
