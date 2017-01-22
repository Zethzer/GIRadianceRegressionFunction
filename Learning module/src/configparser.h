#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <vector>
#include <string>
#include <exception>

#include "opennn.h"
#include "neuralnetworkparameters.h"
#include "datasetparameters.h"

/*
 * This class will parse a config file containing Neural Network
 * parameters and Data Set parameters.
 * The config file must be a .xml file,
 * Details on the structure of this file are given at the bottom.
 * */
class ConfigParser
{
public:
    ConfigParser();

    bool parseConfig(const std::string &config_file_path, NeuralNetworkParameters &neural_network_parameters, DataSetParameters &data_set_parameters);

private:
    //  NeuralNetworkParameters part
    void parseNetwork(tinyxml2::XMLElement *p_network, NeuralNetworkParameters &neural_network_parameters);
    Vector<size_t> parseNeuralNetworkArchitecture(tinyxml2::XMLElement *p_network);
    Vector<ActivationFunction> parseLayersActivationFunctions(tinyxml2::XMLElement *p_network);
    Vector<std::string> parseVariablesNames(tinyxml2::XMLElement *p_network);
    ScalingMethod parseScalingLayerMethod(tinyxml2::XMLElement *p_network);
    UnscalingMethod parseUnscalingLayerMethod(tinyxml2::XMLElement *p_network);

    //  DataSetParameters part
    void parseDataSet(tinyxml2::XMLElement *p_data_set, DataSetParameters &data_set_parameters);
    SplittingParameters parseSplitting(tinyxml2::XMLElement *p_splitting);
    TrainingParameters parseTraining(tinyxml2::XMLElement *p_training);

    //  Utilities
    std::vector<std::string> splitValues(const char *values);
    void checkNode(const tinyxml2::XMLNode *pNode);
    const char *getNodeText(tinyxml2::XMLElement *p_node, const char *node_name);
};

/*
 * The config file must have the following structure:
 *
 * <Config>
 *  <Network>
 *      <Architecture></Architecture>
 *      <LayersActivationFunctions></LayersActivationFunctions>
 *      <InputsNames></InputsNames>
 *      <OutputsNames></OutputsNames>
 *      <ScalingLayerMethod></ScalingLayerMethod>
 *      <UnscalingLayerMethod></UnscalingLayerMethod>
 *  </Network>
 *  <DataSet>
 *      <Splitting>
 *          <SplittingMethod></SplittingMethod>
 *          <SelectionRatio></SelectionRatio>
 *          <TestingRatio></TestingRatio>
 *      </Splitting>
 *      <Training>
 *          <ScalingMethod></ScalingMethod>
 *          <TrainingMethod></TrainingMethod>
 *          <GradientNormGoal></GradientNormGoal>
 *      </Training>
 *  </DataSet>
 * </Config>
 *
 * Architecture: list of int, at least 2. 1st one is the input layer, last one is the outputs layer, other ones are hidden layers.
 * LayersActivationFunctions: [Threshold | SymmetricThreshold | Logistic | HyperbolicTangent | Linear] * number of layers - 1
 * InputsNames: list of strings
 * OutputsNames: list of strings
 * ScalingLayerMethod: NoScaling | MinimumMaximum | MeanStandardDeviation
 * UnscalingLayerMethod: NoUnscaling | MinimumMaximum | MeanStandardDeviation
 * SplittingMethod: Random | Sequential
 * SelectionRatio: double value, 0 <= value <= 1
 * TestingRatio: double value, 0 <= value <= 1, (selection_value + testing_value) <= 1
 * ScalingMethod: MinimumMaximum | MeanStandardDeviation
 * TrainingMethod: QuasiNewton | LevenbergMarquardt | GradientDescent | ConjugateDescent | NewtonMethod
 * GradientNormGoal: double value
 * */

#endif // CONFIGPARSER_H
