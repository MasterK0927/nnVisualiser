/**
 * @file LossFunctions.cpp
 * @brief Implementation of loss functions
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "core/LossFunctions.hpp"

namespace nnv {
namespace core {

// Explicit template instantiations for common types
namespace loss {

// Float instantiations
template float meanSquaredError<float>(const std::vector<float>&, const std::vector<float>&);
template std::vector<float> meanSquaredErrorGradient<float>(const std::vector<float>&, const std::vector<float>&);
template float crossEntropy<float>(const std::vector<float>&, const std::vector<float>&);
template std::vector<float> crossEntropyGradient<float>(const std::vector<float>&, const std::vector<float>&);
template float binaryCrossEntropy<float>(const std::vector<float>&, const std::vector<float>&);
template std::vector<float> binaryCrossEntropyGradient<float>(const std::vector<float>&, const std::vector<float>&);
template float huberLoss<float>(const std::vector<float>&, const std::vector<float>&, float);
template std::vector<float> huberLossGradient<float>(const std::vector<float>&, const std::vector<float>&, float);
template float focalLoss<float>(const std::vector<float>&, const std::vector<float>&, float, float);
template std::vector<float> focalLossGradient<float>(const std::vector<float>&, const std::vector<float>&, float, float);

// Double instantiations
template double meanSquaredError<double>(const std::vector<double>&, const std::vector<double>&);
template std::vector<double> meanSquaredErrorGradient<double>(const std::vector<double>&, const std::vector<double>&);
template double crossEntropy<double>(const std::vector<double>&, const std::vector<double>&);
template std::vector<double> crossEntropyGradient<double>(const std::vector<double>&, const std::vector<double>&);
template double binaryCrossEntropy<double>(const std::vector<double>&, const std::vector<double>&);
template std::vector<double> binaryCrossEntropyGradient<double>(const std::vector<double>&, const std::vector<double>&);
template double huberLoss<double>(const std::vector<double>&, const std::vector<double>&, double);
template std::vector<double> huberLossGradient<double>(const std::vector<double>&, const std::vector<double>&, double);
template double focalLoss<double>(const std::vector<double>&, const std::vector<double>&, double, double);
template std::vector<double> focalLossGradient<double>(const std::vector<double>&, const std::vector<double>&, double, double);

} // namespace loss

// LossFactory template instantiations
template std::function<float(const std::vector<float>&, const std::vector<float>&)> 
LossFactory::getFunction<float>(LossType);

template std::function<std::vector<float>(const std::vector<float>&, const std::vector<float>&)> 
LossFactory::getGradient<float>(LossType);

template std::function<double(const std::vector<double>&, const std::vector<double>&)> 
LossFactory::getFunction<double>(LossType);

template std::function<std::vector<double>(const std::vector<double>&, const std::vector<double>&)> 
LossFactory::getGradient<double>(LossType);

} // namespace core
} // namespace nnv
