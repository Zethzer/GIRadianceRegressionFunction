#ifndef LEARNINGMODULE_H
#define LEARNINGMODULE_H

#include <iostream>
#include <stdexcept>
#include <algorithm>

#include "neuralnetworkarchitecture.h"

enum TrainingMethod
{
    QuasiNewton,
    LevenbergMarquardt
};

struct LearningParameters
{
    TrainingMethod m_training_method;
    double m_max_error;

    void init(const TrainingMethod &training_method, const double &max_error)
    {
        m_training_method = training_method;
        m_max_error = max_error;
    }
};

/*
 * Module that will proceed to the learning
 * of a neural network using a data set
 * */
class LearningModule
{
public:
    LearningModule(const NeuralNetworkArchitecture &neural_network_architecture, const LearningParameters &learning_parameter);

    void trainNetwork(const std::string &data_set_path);

    inline void saveNetwork(const std::string &neural_network_save_path) const{m_neural_network->save(neural_network_save_path);}

private:
    NeuralNetworkArchitecture m_neural_network_architecture;
    LearningParameters m_learning_parameters;

    DataSet m_data_set;
    NeuralNetwork *m_neural_network;

    std::string saved_data_set_path;
};

#endif // LEARNINGMODULE_H
