#ifndef NEURALNETWORK_H
#define NEURALNETWORK_H

/*
 * This file gives structures and defines that describes a Neural Network.
 * The created NeuralNetwork can be send to a compute shader with inputs,
 * that will be evaluated.
 *
 * We only described ScalingLayer, UnscalingLayer and MultilayerPerceptron
 * because we don't used the other structures for our purpose.
 * They can easily be added, as the structures are inspired of OpenNN structure
 * */


// The following defines will be used in the compute shader

//  (Un)Scaling Methods
#define NoScaling 0
#define MinimumMaximum 1
#define MeanStandardDeviation 2

//  Activation Functions
#define Linear 0
#define HyperbolicTangent 1

#define MAX_INPUTS_SIZE 12
#define MAX_STATISTICS_SIZE 20
#define MAX_MULTILAYER_SIZE 4
#define MAX_LAYER_SIZE 20


//  Structures that describe a Neural Network, will be used to evaluate inputs
//  Pointers mean arrays

struct Perceptron
{
    unsigned int inputs_number;

    int activation_function;
    float bias;
    float synaptic_weights[20];
};

struct PerceptronLayer
{
    unsigned int perceptrons_size;

	Perceptron perceptrons[20];
	size_t get_perceptrons_number(void) const
	{
		return 20;
	}
	//  Layer
	Vector<double> PerceptronLayerCalculateOutputs(const Vector<double>& inputs) const
	{
		return(calculate_activations(calculate_combinations(inputs)));
	}
};

struct MultilayerPerceptron
{
	unsigned int layers_size;

	PerceptronLayer perceptron_layers[20];
	size_t get_layers_number(void) const
	{
		return 20;
	}
};

struct Statistics
{
    float minimum;
    float maximum;
    float mean;
    float standard_deviation;
};

struct ScalingLayer
{
    unsigned int statistics_size;

    int scaling_method;
    Statistics statistics[20];
	size_t get_scaling_neurons_number(void) const
	{
		return 20;
	}
};

struct UnscalingLayer
{
	unsigned int statistics_size;

	int unscaling_method;
	Statistics statistics[20];
};

struct NeuralNetwork
{
    ScalingLayer scaling_layer;
    MultilayerPerceptron multilayer_perceptron;
    UnscalingLayer unscaling_layer;
};

#endif // NEURALNETWORK_H
