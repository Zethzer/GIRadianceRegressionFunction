#include "learningmodule.h"

LearningModule::LearningModule(const NeuralNetworkArchitecture &neural_network_architecture, const LearningParameters &learning_parameter) :
    m_neural_network_architecture(neural_network_architecture),
    m_learning_parameters(learning_parameter)
{
    m_neural_network = new NeuralNetwork(m_neural_network_architecture.m_network_architecture);
}

/*
 * Extract DataSet from a file which path is 'data_set_path'
 * the file must respect the architecture of the network
 *
 * then, train the network with the DataSet
 * */
void LearningModule::trainNetwork(const std::string &data_set_path)
{
    try
    {
        DataSet data_set;
        data_set.set_data_file_name(data_set_path);
        data_set.load_data();

        Variables* variables_pointer = data_set.get_variables_pointer();

        size_t i = 0;
        for(; i < m_neural_network_architecture.m_number_of_inputs; ++i)
            variables_pointer->set_use(i, Variables::Input);

        for(; i < m_neural_network_architecture.m_number_of_inputs + m_neural_network_architecture.m_number_of_outputs; ++i)
            variables_pointer->set_use(i, Variables::Target);

        for(i = 0; i < m_neural_network_architecture.m_variables_names.size(); ++i)
            variables_pointer->set_name(i, m_neural_network_architecture.m_variables_names[i]);

        Matrix<std::string> inputs_information = variables_pointer->arrange_inputs_information();
        Matrix<std::string> targets_information = variables_pointer->arrange_targets_information();

        Instances* instances_pointer = data_set.get_instances_pointer();
        instances_pointer->set_training();

        Vector< Statistics<double> > inputs_statistics = data_set.scale_inputs_minimum_maximum();
        Vector< Statistics<double> > targets_statistics = data_set.scale_targets_minimum_maximum();

        Inputs* inputs_pointer = m_neural_network->get_inputs_pointer();
        inputs_pointer->set_information(inputs_information);

        Outputs* outputs_pointer = m_neural_network->get_outputs_pointer();
        outputs_pointer->set_information(targets_information);


        m_neural_network->construct_scaling_layer();
        ScalingLayer* scaling_layer_pointer = m_neural_network->get_scaling_layer_pointer();
        scaling_layer_pointer->set_statistics(inputs_statistics);
        scaling_layer_pointer->set_scaling_method(m_neural_network_architecture.m_scaling_method);

        m_neural_network->construct_unscaling_layer();
        UnscalingLayer* unscaling_layer_pointer = m_neural_network->get_unscaling_layer_pointer();
        unscaling_layer_pointer->set_statistics(targets_statistics);
        unscaling_layer_pointer->set_unscaling_method(m_neural_network_architecture.m_unscaling_method);


        PerformanceFunctional performance_functional(m_neural_network, &m_data_set);
        TrainingStrategy training_strategy(&performance_functional);

        if(m_learning_parameters.m_training_method == QuasiNewton)
        {
            QuasiNewtonMethod* quasi_Newton_method_pointer = training_strategy.get_quasi_Newton_method_pointer();
            quasi_Newton_method_pointer->set_performance_goal(m_learning_parameters.m_max_error);
        }
        else if(m_learning_parameters.m_training_method == LevenbergMarquardt)
        {
            LevenbergMarquardtAlgorithm* levenberq_marquardt_method_pointer = training_strategy.get_Levenberg_Marquardt_algorithm_pointer();
            levenberq_marquardt_method_pointer->set_performance_goal(m_learning_parameters.m_max_error);
        }

        training_strategy.perform_training();
    }
    catch(std::exception& e)
    {
        std::cerr << "LEARNINGMODULE::TRAINNETWORK::" << e.what() << std::endl;
    }
}
