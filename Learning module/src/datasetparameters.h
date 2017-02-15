#ifndef DATASETPARAMETERS_H
#define DATASETPARAMETERS_H

#include "opennn.h"

typedef OpenNN::Instances::SplittingMethod SplittingMethod;
typedef OpenNN::TrainingStrategy::MainType TrainingMethod;

struct DataSetParameters
{
    struct SplittingParameters
    {
        SplittingMethod m_splitting_method;

        double m_training_data_ratio;
        double m_selection_data_ratio;
        double m_testing_data_ratio;

        SplittingParameters()
        {

        }

        SplittingParameters(const SplittingMethod &splitting_method, const double &training_data_ratio, const double &selection_data_ratio, const double &testing_data_ratio) :
            m_splitting_method(splitting_method),
            m_training_data_ratio(training_data_ratio),
            m_selection_data_ratio(selection_data_ratio),
            m_testing_data_ratio(testing_data_ratio)
        {

        }
    };
    struct TrainingParameters
    {
        enum DataScalingMethod
        {
            MinimumMaximum,
            MeanStandardDeviation
        };

        DataScalingMethod m_data_scaling_method;
        TrainingMethod m_training_method;
        double m_gradient_norm_goal;

        TrainingParameters()
        {

        }

        TrainingParameters(const DataScalingMethod &data_scaling_method, const TrainingMethod &training_method, const double &gradient_norm_goal) :
            m_data_scaling_method(data_scaling_method),
            m_training_method(training_method),
            m_gradient_norm_goal(gradient_norm_goal)
        {

        }
    };

    SplittingParameters m_splitting_parameters;
    TrainingParameters m_training_parameters;

    void init(const SplittingParameters &splitting_parameters, const TrainingParameters &training_parameters)
    {
        m_splitting_parameters = splitting_parameters;
        m_training_parameters = training_parameters;
    }
};

typedef DataSetParameters::TrainingParameters::DataScalingMethod DataScalingMethod;
typedef DataSetParameters::SplittingParameters SplittingParameters;
typedef DataSetParameters::TrainingParameters TrainingParameters;

#endif // DATASETPARAMETERS_H
