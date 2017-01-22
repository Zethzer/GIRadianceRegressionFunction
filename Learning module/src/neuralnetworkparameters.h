#ifndef NEURALNETWORKPARAMETERS_H
#define NEURALNETWORKPARAMETERS_H

#include "opennn.h"

using namespace OpenNN;

typedef Vector<double> Input;
typedef Vector<double> Output;

typedef ScalingLayer::ScalingMethod ScalingMethod;
typedef UnscalingLayer::UnscalingMethod UnscalingMethod;
typedef Perceptron::ActivationFunction ActivationFunction;

/*
 * Defines the architecture of a neural network
 * the number of layers and the number of neurons in each layer
 * the names of variables
 * the number of inputs/outputs
 * */
struct NeuralNetworkParameters
{
    Vector<size_t> m_network_architecture;
    Vector<Perceptron::ActivationFunction> m_layers_activation_functions;
    Vector<std::string> m_variables_names;

    size_t m_number_of_inputs;
    size_t m_number_of_outputs;

    ScalingLayer::ScalingMethod m_scaling_layer_method;
    UnscalingLayer::UnscalingMethod m_unscaling_layer_method;

    void init(const Vector<size_t> &network_architecture, const Vector<Perceptron::ActivationFunction> &layers_activation_functions, const Vector<std::string> &variables_names, const ScalingLayer::ScalingMethod &scaling_layer_method, const UnscalingLayer::UnscalingMethod &unscaling_layer_method)
    {
        m_network_architecture = network_architecture;
        m_layers_activation_functions = layers_activation_functions;
        m_variables_names = variables_names;
        m_scaling_layer_method = scaling_layer_method;
        m_unscaling_layer_method = unscaling_layer_method;
        m_number_of_inputs = m_network_architecture[0];
        m_number_of_outputs = m_network_architecture.back();
    }
};

#endif // NEURALNETWORKPARAMETERS_H
