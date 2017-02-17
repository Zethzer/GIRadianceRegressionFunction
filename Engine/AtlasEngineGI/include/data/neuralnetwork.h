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
#define MAX_STATISTICS_SIZE x
#define MAX_MULTILAYER_SIZE 4
#define MAX_LAYER_SIZE 20


//  Structures that describe a Neural Network, will be used to evaluate inputs
//  Pointers mean arrays

struct Perceptron
{
    unsigned int inputs_number;

    int activation_function;
    float bias;
    float *synaptic_weights;

    Perceptron() :
        synaptic_weights(0)
    {
    }

    ~Perceptron()
    {
        if(synaptic_weights)
            delete[] synaptic_weights;
    }
};

struct PerceptronLayer
{
    unsigned int perceptrons_size;

    Perceptron *perceptrons;

    PerceptronLayer() :
        perceptrons(0)
    {
    }

    ~PerceptronLayer()
    {
        if(perceptrons)
            delete[] perceptrons;
    }
};

struct MultilayerPerceptron
{
    unsigned int layers_size;

    PerceptronLayer *perceptron_layers;

    MultilayerPerceptron() :
        perceptron_layers(0)
    {
    }

    ~MultilayerPerceptron()
    {
        if(perceptron_layers)
            delete[] perceptron_layers;
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
    Statistics *statistics;

    ScalingLayer() :
        statistics(0)
    {
    }

    ~ScalingLayer()
    {
        if(statistics)
            delete[] statistics;
    }
};

struct UnscalingLayer
{
    unsigned int statistics_size;

    int unscaling_method;
    Statistics *statistics;

    UnscalingLayer() :
        statistics(0)
    {
    }

    ~UnscalingLayer()
    {
        if(statistics)
            delete[] statistics;
    }
};

struct NeuralNetwork
{
    ScalingLayer scaling_layer;
    MultilayerPerceptron multilayer_perceptron;
    UnscalingLayer unscaling_layer;
};

#endif // NEURALNETWORK_H
