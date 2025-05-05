/**
 * @file ActivationFunctions.cpp
 * @brief Implementation of activation functions
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "core/ActivationFunctions.hpp"

namespace nnv {
namespace core {

// Explicit template instantiations for common types
namespace activation {

// Float instantiations
template float linear<float>(float);
template float linearDerivative<float>(float);
template float relu<float>(float);
template float reluDerivative<float>(float);
template float leakyRelu<float>(float, float);
template float leakyReluDerivative<float>(float, float);
template float sigmoid<float>(float);
template float sigmoidDerivative<float>(float);
template float tanh<float>(float);
template float tanhDerivative<float>(float);
template float elu<float>(float, float);
template float eluDerivative<float>(float, float);
template float swish<float>(float);
template float swishDerivative<float>(float);
template float gelu<float>(float);
template float geluDerivative<float>(float);
template std::vector<float> softmax<float>(const std::vector<float>&);
template float softmaxDerivative<float>(const std::vector<float>&, std::size_t, std::size_t);

// Double instantiations
template double linear<double>(double);
template double linearDerivative<double>(double);
template double relu<double>(double);
template double reluDerivative<double>(double);
template double leakyRelu<double>(double, double);
template double leakyReluDerivative<double>(double, double);
template double sigmoid<double>(double);
template double sigmoidDerivative<double>(double);
template double tanh<double>(double);
template double tanhDerivative<double>(double);
template double elu<double>(double, double);
template double eluDerivative<double>(double, double);
template double swish<double>(double);
template double swishDerivative<double>(double);
template double gelu<double>(double);
template double geluDerivative<double>(double);
template std::vector<double> softmax<double>(const std::vector<double>&);
template double softmaxDerivative<double>(const std::vector<double>&, std::size_t, std::size_t);

} // namespace activation

// ActivationFactory template instantiations
template std::function<float(float)> ActivationFactory::getFunction<float>(ActivationType);
template std::function<float(float)> ActivationFactory::getDerivative<float>(ActivationType);
template std::function<double(double)> ActivationFactory::getFunction<double>(ActivationType);
template std::function<double(double)> ActivationFactory::getDerivative<double>(ActivationType);

} // namespace core
} // namespace nnv
