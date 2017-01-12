#ifndef NEURALNETWORKARCHITECTURE_H
#define NEURALNETWORKARCHITECTURE_H

#include "../opennn/opennn.h"

using namespace OpenNN;

typedef Vector<double> Input;
typedef Vector<double> Output;

/*
 * Defines the architecture of a neural network
 * the number of layers and the number of neurons in each layer
 * the names of variables
 * the number of inputs/outputs
 * */
struct NeuralNetworkArchitecture
{
    Vector<size_t> m_network_architecture;
    Vector<std::string> m_variables_names;

    size_t m_number_of_inputs;
    size_t m_number_of_outputs;

    NeuralNetworkArchitecture(const Vector<size_t> &network_architecture, const Vector<std::string> &variables_names) :
        m_network_architecture(network_architecture),
        m_variables_names(variables_names)
    {
        m_number_of_inputs = m_network_architecture[0];
        m_number_of_outputs = m_network_architecture.back();
    }
};

#endif // NEURALNETWORKARCHITECTURE_H
