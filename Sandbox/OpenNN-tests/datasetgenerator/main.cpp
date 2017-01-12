#include <time.h>
#include <sstream>
#include <fstream>

#include "../neuralnetworkarchitecture.h"

int main(int argc, char *argv[])
{
    srand( (unsigned)time( NULL ) );
    std::cout << "hello world" << std::endl;
}

/*
 * Generate 'm_number_of_datas' random datas between
 * 'm_low' and 'm_high' following OpenNN format
 * Generate outputs which are linear combination of inputs
 * Saves the result in the file 'm_data_set_path'
 * */
void generateDataSet(const NeuralNetworkArchitecture &neural_network_architecture, const std::string &data_set_path, const size_t &number_of_datas, const double &low, const double &high)
{
    std::ofstream stream(data_set_path);

    double *data = new double[neural_network_architecture.m_number_of_inputs + neural_network_architecture.m_number_of_outputs];

    for(size_t i = 0; i < number_of_datas; ++i)
    {
        for(size_t j = 0; j < neural_network_architecture.m_number_of_inputs; ++j)
            data[j] = low + static_cast <double> (rand()) /( static_cast <double> (RAND_MAX/(high-low)));

        for(size_t j = 0; j < neural_network_architecture.m_number_of_outputs; ++j)
        {
            for(size_t k = 0; k < neural_network_architecture.m_number_of_inputs; ++k)
            {
                int factor = rand() % 2;
                data[neural_network_architecture.m_number_of_inputs + j] += factor * data[k];
            }
        }

        stream << data[0];
        for(size_t j = 1; j < neural_network_architecture.m_number_of_inputs + neural_network_architecture.m_number_of_outputs; ++j)
            stream << ' ' << data[j];
        stream << std::endl;
    }

    stream.close();
}
