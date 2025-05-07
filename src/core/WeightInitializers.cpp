/**
 * @file WeightInitializers.cpp
 * @brief Implementation of weight initializers
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "core/WeightInitializers.hpp"

namespace nnv {
namespace core {

// Explicit template instantiations for common types
namespace initializer {

// Float instantiations
template float xavier<float>(std::size_t, std::size_t);
template float he<float>(std::size_t);
template float randomUniform<float>(float, float);
template float randomNormal<float>(float, float);
template float constant<float>(float);
template float zeros<float>();
template float ones<float>();
template float lecun<float>(std::size_t);
template std::vector<std::vector<float>> orthogonal<float>(std::pair<std::size_t, std::size_t>, float);

// Double instantiations
template double xavier<double>(std::size_t, std::size_t);
template double he<double>(std::size_t);
template double randomUniform<double>(double, double);
template double randomNormal<double>(double, double);
template double constant<double>(double);
template double zeros<double>();
template double ones<double>();
template double lecun<double>(std::size_t);
template std::vector<std::vector<double>> orthogonal<double>(std::pair<std::size_t, std::size_t>, double);

} // namespace initializer

// InitializerFactory template instantiations
template std::function<float(std::size_t, std::size_t)> 
InitializerFactory::getFunction<float>(InitializationType);

template std::function<double(std::size_t, std::size_t)> 
InitializerFactory::getFunction<double>(InitializationType);

} // namespace core
} // namespace nnv
