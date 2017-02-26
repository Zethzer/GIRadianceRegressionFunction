#include "opennn.h"
#include "../opennn/neural_network.h"
#include "../neuralnetworkparameters.h"

using namespace OpenNN;

Vector<Output> evaluate(const std::string &neural_network_path, Vector<Input> inputs);
NeuralNetwork loadNeuralNetwork(const std::string &neural_network_path);
Output evaluateInput(NeuralNetwork &neural_network, const Input &input);

int main(int argc, char *argv[])
{
    NeuralNetwork nn = loadNeuralNetwork("neuralnetwork.xml");
    Input input;
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);
    input.push_back(1.0);

    Output output = evaluateInput(nn, input);

    for(size_t i = 0; i < output.size(); ++i)
        std::cout << output[i] << ' ';
}

Vector<Output> evaluate(const std::string &neural_network_path, Vector<Input> inputs)
{
    NeuralNetwork neural_network = loadNeuralNetwork(neural_network_path);

    Vector<Output> evaluated_outputs;

    for(size_t i = 0; i < inputs.size(); ++i)
        evaluated_outputs.push_back(evaluateInput(neural_network, inputs[i]));

    return evaluated_outputs;
}

/*
 * Load a neural network using
 * saved path
 * */
NeuralNetwork loadNeuralNetwork(const std::string &neural_network_path)
{
    try
    {
        NeuralNetwork neural_network;
        neural_network.load(neural_network_path);
        return neural_network;
    }
    catch(std::exception& e)
    {
       std::cerr << "DATASETEVALUATOR::LOADNEURALNETWORK::ERROR" << e.what() << std::endl;

       return NeuralNetwork();
    }
}


/*
 * Evaluate the Input 'input'
 * using the NeuralNework 'nn'
 * and return the Output result
 * */
Output evaluateInput(NeuralNetwork &neural_network, const Input &input)
{
    try
    {
       return neural_network.calculate_outputs(input);
    }
    catch(std::exception& e)
    {
       std::cerr << "DATASETEVALUATOR::EVALUATEINPUT::ERROR" << e.what() << std::endl;

       return Output();
    }
}
