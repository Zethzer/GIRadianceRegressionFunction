#include "../neuralnetworkarchitecture.h"

#include "iostream"

/*
 * Inputs :
 *
 * - DataSet1 path, .dat file
 * - DataSet2 path, .dat file
 *
 * Outputs :
 *
 * Compute and print the average, the miniumu
 * and the maximum gaps between both ouptuts
 * of DataSets
 * */
int main(int argc, char *argv[])
{
    std::cout << "hello world" << std::endl;
}

bool extractArguments( NeuralNetworkArchitecture &neural_network_architecture, Vector<Output> &outputs1, Vector<Output> &outputs2)
{

}


/*
 * Compare Outputs outputs1 and outputs2
 * computing the minimum, maximum and average gap
 * between them
 * and print the result
 * */
void compare(const NeuralNetworkArchitecture &neural_network_architecture, const Vector<Output> &outputs1, const Vector<Output> &outputs2)
{
    if(outputs1.size() != outputs2.size())
    {
        std::cerr << "DATASETCOMPARATOR::COMPARE::ERROR outputs to compare must have the same size" << std::endl;

        return;
    }

    double *average = new double[neural_network_architecture.m_number_of_outputs],
           *maximum = new double[neural_network_architecture.m_number_of_outputs],
           *minimum = new double[neural_network_architecture.m_number_of_outputs];

    memset(&average[0], 0, neural_network_architecture.m_number_of_outputs * sizeof average[0]);
    memset(&maximum[0], 0, neural_network_architecture.m_number_of_outputs * sizeof maximum[0]);
    memset(&minimum[0], INFINITY, neural_network_architecture.m_number_of_outputs * sizeof minimum[0]);

    for(size_t i = 0; i < outputs1.size(); ++i)
    {
        double gaps[neural_network_architecture.m_number_of_outputs];

        for(size_t j = 0; j < neural_network_architecture.m_number_of_outputs; ++j)
        {
            gaps[0] = fabs(outputs1[i][0] - outputs2[i][0]);
            maximum[j] = std::max(maximum[j], gaps[j]);
            minimum[j] = std::min(minimum[j], gaps[j]);
            average[j] += gaps[j];
        }
    }

    for(size_t i = 0; i < neural_network_architecture.m_number_of_outputs; ++i)
        average[i] /= outputs1.size();

    std::cout << std::endl << "Results : "

    << std::endl << "minimum : ";
    for(size_t i = 0; i < neural_network_architecture.m_number_of_outputs; ++i)
        std::cout << minimum[i] << ' ';

    std::cout << std::endl << "maximum : ";
    for(size_t i = 0; i < neural_network_architecture.m_number_of_outputs; ++i)
        std::cout << maximum[i];

    std::cout << std::endl << "average : ";
    for(size_t i = 0; i < neural_network_architecture.m_number_of_outputs; ++i)
        std::cout << average[i];

    std::cout << std::endl;
}

/*
 * Extract datas from file 'data_set_path'
 * respecting the neural_network_architecture
 * and save the inputs and outputs respectively in
 * vectors 'inputs' and 'outputs'
 * */
void extractDatas(const NeuralNetworkArchitecture &neural_network_architecture, const std::string &data_set_path, Vector<Input> &inputs, Vector<Output> &outputs)
{
    DataSet data_set;
    data_set.set_data_file_name(data_set_path);
    data_set.load_data();

    const Matrix<double> datas = data_set.get_data();

    if((neural_network_architecture.m_number_of_inputs + neural_network_architecture.m_number_of_outputs) != datas.get_columns_number())
    {
        std::cerr << "EXTRACTDATAS::ERROR network architecture and dataset are incompatible" << std::endl;

        return;
    }

    for(size_t i = 0; i < neural_network_architecture.m_number_of_inputs; ++i)
            inputs.push_back(datas[i]);

    for(size_t i = 0; i < neural_network_architecture.m_number_of_outputs; ++i)
            outputs.push_back(datas[i]);
}
