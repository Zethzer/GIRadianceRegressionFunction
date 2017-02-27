#ifndef TRAINER_H
#define TRAINER_H

#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "neuralnetworkparameters.h"
#include "datasetparameters.h"

/*
 * This class will proceed to the training
 * of a neural network using data set(s)
 *
 * Trainer has to be initialized with NeuralNetworkArchitecture
 * in order to create the NeuralNetwork.
 *
 * Then, the network can be trained using a DataSet, by sending
 * its path, and DataSetParameters.
 *
 * NeuralNetwork can also be saved.
 * */
class Trainer
{
public:
    Trainer();

    void init(const NeuralNetworkParameters &neural_network_parameters);
    void init(const std::string &neural_network_file_path);

    void trainNetwork(DataSet &data_set, DataSetParameters &data_set_parameters);
    inline void saveNetwork(const std::string &neural_network_save_path) const {m_neural_network->save(neural_network_save_path);}

private:
    DataSet loadDataSet(const std::string &data_set_path);
    NeuralNetwork loadNeuralNetwork(const std::string &neural_network_path);

    NeuralNetworkParameters m_neural_network_parameters;

    DataSet m_data_set;
    NeuralNetwork *m_neural_network;

    std::string saved_data_set_path;

    ScalingLayer *m_scaling_layer_pointer;
    UnscalingLayer *m_unscaling_layer_pointer;
};

#endif // TRAINER_H
