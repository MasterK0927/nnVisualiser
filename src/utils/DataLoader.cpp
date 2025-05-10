/**
 * @file DataLoader.cpp
 * @brief Implementation of data loading utilities
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#include "utils/DataLoader.hpp"
#include "utils/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <filesystem>
#include <cstring>

#ifdef HAS_OPENCV
#include <opencv2/opencv.hpp>
#endif

namespace nnv {
namespace utils {

template<typename T>
std::pair<Dataset<T>, Dataset<T>> Dataset<T>::split(float validationRatio) const {
    if (validationRatio <= 0.0f || validationRatio >= 1.0f) {
        return {*this, Dataset<T>{}};
    }
    
    std::size_t valSize = static_cast<std::size_t>(size() * validationRatio);
    std::size_t trainSize = size() - valSize;
    
    Dataset<T> trainSet, valSet;
    
    // Copy training data
    trainSet.inputs.assign(inputs.begin(), inputs.begin() + trainSize);
    trainSet.targets.assign(targets.begin(), targets.begin() + trainSize);
    if (!labels.empty()) {
        trainSet.labels.assign(labels.begin(), labels.begin() + trainSize);
    }
    
    // Copy validation data
    valSet.inputs.assign(inputs.begin() + trainSize, inputs.end());
    valSet.targets.assign(targets.begin() + trainSize, targets.end());
    if (!labels.empty()) {
        valSet.labels.assign(labels.begin() + trainSize, labels.end());
    }
    
    // Copy label map
    trainSet.labelMap = labelMap;
    valSet.labelMap = labelMap;
    
    return {trainSet, valSet};
}

template<typename T>
Dataset<T> DataLoader<T>::loadFromFile(const std::string& filename,
                                      DataFormat format,
                                      const PreprocessingConfig& config) {
    if (format == DataFormat::CSV) {
        format = detectFormat(filename);
    }
    
    Dataset<T> dataset;
    
    try {
        switch (format) {
            case DataFormat::CSV:
                dataset = loadCSV(filename);
                break;
            case DataFormat::Image:
                // For single image, create dataset with one sample
                {
                    auto imageData = loadImage(filename, config);
                    if (!imageData.empty()) {
                        dataset.inputs.push_back(imageData);
                        dataset.targets.push_back({T{0}}); // Default target
                    }
                }
                break;
            default:
                NNV_LOG_ERROR("Unsupported data format for file: {}", filename);
                return dataset;
        }
        
        if (!dataset.empty()) {
            preprocess(dataset, config);
        }
        
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to load data from {}: {}", filename, e.what());
    }
    
    return dataset;
}

template<typename T>
Dataset<T> DataLoader<T>::loadCSV(const std::string& filename,
                                 bool hasHeader,
                                 char delimiter,
                                 int targetColumn) {
    Dataset<T> dataset;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        NNV_LOG_ERROR("Failed to open CSV file: {}", filename);
        return dataset;
    }
    
    std::string line;
    bool firstLine = true;
    std::vector<std::string> headers;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        auto values = parseCSVLine(line, delimiter);
        if (values.empty()) continue;
        
        if (firstLine && hasHeader) {
            headers = values;
            firstLine = false;
            continue;
        }
        
        // Determine target column
        int targetCol = targetColumn;
        if (targetCol < 0) {
            targetCol = static_cast<int>(values.size()) - 1;
        }
        
        // Parse input features
        std::vector<T> input;
        for (int i = 0; i < static_cast<int>(values.size()); ++i) {
            if (i != targetCol) {
                try {
                    input.push_back(static_cast<T>(std::stod(values[i])));
                } catch (const std::exception&) {
                    NNV_LOG_WARNING("Failed to parse value '{}' as number", values[i]);
                    input.push_back(T{0});
                }
            }
        }
        
        // Parse target
        std::vector<T> target;
        if (targetCol < static_cast<int>(values.size())) {
            try {
                target.push_back(static_cast<T>(std::stod(values[targetCol])));
            } catch (const std::exception&) {
                // Try to parse as categorical label
                std::string label = values[targetCol];
                auto it = dataset.labelMap.find(label);
                if (it == dataset.labelMap.end()) {
                    int labelIndex = static_cast<int>(dataset.labelMap.size());
                    dataset.labelMap[label] = labelIndex;
                    target.push_back(static_cast<T>(labelIndex));
                } else {
                    target.push_back(static_cast<T>(it->second));
                }
                dataset.labels.push_back(label);
            }
        }
        
        if (!input.empty() && !target.empty()) {
            dataset.inputs.push_back(input);
            dataset.targets.push_back(target);
        }
        
        firstLine = false;
    }
    
    NNV_LOG_INFO("Loaded {} samples from CSV file: {}", dataset.size(), filename);
    return dataset;
}

template<typename T>
Dataset<T> DataLoader<T>::loadMNIST(const std::string& imagesFile,
                                   const std::string& labelsFile) {
    Dataset<T> dataset;
    
    try {
        auto images = readMNISTImages(imagesFile);
        auto labels = readMNISTLabels(labelsFile);
        
        if (images.size() != labels.size()) {
            NNV_LOG_ERROR("MNIST images and labels count mismatch: {} vs {}", 
                         images.size(), labels.size());
            return dataset;
        }
        
        dataset.inputs = std::move(images);
        dataset.targets = oneHotEncode(labels, 10); // MNIST has 10 classes
        
        // Create label map
        for (int i = 0; i < 10; ++i) {
            dataset.labelMap[std::to_string(i)] = i;
        }
        
        NNV_LOG_INFO("Loaded {} MNIST samples", dataset.size());
        
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to load MNIST data: {}", e.what());
    }
    
    return dataset;
}

template<typename T>
Dataset<T> DataLoader<T>::loadImagesFromDirectory(const std::string& directory,
                                                 const PreprocessingConfig& config) {
    Dataset<T> dataset;
    
    if (!std::filesystem::exists(directory)) {
        NNV_LOG_ERROR("Directory does not exist: {}", directory);
        return dataset;
    }
    
    auto supportedFormats = getSupportedImageFormats();
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (!entry.is_regular_file()) continue;
            
            std::string filename = entry.path().string();
            std::string extension = getFileExtension(filename);
            extension = toLower(extension);
            
            // Check if file format is supported
            if (std::find(supportedFormats.begin(), supportedFormats.end(), extension) == 
                supportedFormats.end()) {
                continue;
            }
            
            // Load image
            auto imageData = loadImage(filename, config);
            if (imageData.empty()) continue;
            
            // Use parent directory name as label
            std::string label = entry.path().parent_path().filename().string();
            
            // Add to label map if new
            if (dataset.labelMap.find(label) == dataset.labelMap.end()) {
                dataset.labelMap[label] = static_cast<int>(dataset.labelMap.size());
            }
            
            dataset.inputs.push_back(imageData);
            dataset.targets.push_back({static_cast<T>(dataset.labelMap[label])});
            dataset.labels.push_back(label);
        }
        
        NNV_LOG_INFO("Loaded {} images from directory: {}", dataset.size(), directory);
        
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to load images from directory {}: {}", directory, e.what());
    }
    
    return dataset;
}

template<typename T>
std::vector<T> DataLoader<T>::loadImage(const std::string& filename,
                                       const PreprocessingConfig& config) {
    std::vector<T> imageData;
    
#ifdef HAS_OPENCV
    try {
        cv::Mat image = cv::imread(filename, config.grayscale ? cv::IMREAD_GRAYSCALE : cv::IMREAD_COLOR);
        
        if (image.empty()) {
            NNV_LOG_WARNING("Failed to load image: {}", filename);
            return imageData;
        }
        
        // Resize if needed
        if (config.imageSize.first > 0 && config.imageSize.second > 0) {
            cv::resize(image, image, cv::Size(config.imageSize.first, config.imageSize.second));
        }
        
        // Convert to float and normalize
        image.convertTo(image, CV_32F);
        if (config.normalize) {
            image /= 255.0f;
        }
        
        // Flatten image
        imageData.reserve(image.total() * image.channels());
        for (int i = 0; i < image.rows; ++i) {
            for (int j = 0; j < image.cols; ++j) {
                if (image.channels() == 1) {
                    imageData.push_back(static_cast<T>(image.at<float>(i, j)));
                } else {
                    cv::Vec3f pixel = image.at<cv::Vec3f>(i, j);
                    for (int c = 0; c < 3; ++c) {
                        imageData.push_back(static_cast<T>(pixel[c]));
                    }
                }
            }
        }
        
    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to process image {}: {}", filename, e.what());
    }
#else
    NNV_LOG_WARNING("OpenCV not available, cannot load image: {}", filename);
#endif
    
    return imageData;
}

template<typename T>
void DataLoader<T>::preprocess(Dataset<T>& dataset, const PreprocessingConfig& config) {
    if (dataset.empty()) return;
    
    if (config.shuffle) {
        shuffle(dataset);
    }
    
    if (config.normalize) {
        normalize(dataset.inputs);
    }
    
    if (config.standardize) {
        standardize(dataset.inputs);
    }
    
    NNV_LOG_DEBUG("Applied preprocessing to dataset with {} samples", dataset.size());
}

template<typename T>
void DataLoader<T>::normalize(std::vector<std::vector<T>>& data) {
    if (data.empty()) return;
    
    std::size_t numFeatures = data[0].size();
    
    // Find min and max for each feature
    std::vector<T> minVals(numFeatures, std::numeric_limits<T>::max());
    std::vector<T> maxVals(numFeatures, std::numeric_limits<T>::lowest());
    
    for (const auto& sample : data) {
        for (std::size_t i = 0; i < numFeatures; ++i) {
            minVals[i] = std::min(minVals[i], sample[i]);
            maxVals[i] = std::max(maxVals[i], sample[i]);
        }
    }
    
    // Normalize to [0, 1]
    for (auto& sample : data) {
        for (std::size_t i = 0; i < numFeatures; ++i) {
            T range = maxVals[i] - minVals[i];
            if (range > T{0}) {
                sample[i] = (sample[i] - minVals[i]) / range;
            }
        }
    }
}

template<typename T>
void DataLoader<T>::standardize(std::vector<std::vector<T>>& data) {
    if (data.empty()) return;
    
    std::size_t numFeatures = data[0].size();
    std::size_t numSamples = data.size();
    
    // Calculate mean for each feature
    std::vector<T> means(numFeatures, T{0});
    for (const auto& sample : data) {
        for (std::size_t i = 0; i < numFeatures; ++i) {
            means[i] += sample[i];
        }
    }
    for (auto& mean : means) {
        mean /= static_cast<T>(numSamples);
    }
    
    // Calculate standard deviation for each feature
    std::vector<T> stds(numFeatures, T{0});
    for (const auto& sample : data) {
        for (std::size_t i = 0; i < numFeatures; ++i) {
            T diff = sample[i] - means[i];
            stds[i] += diff * diff;
        }
    }
    for (auto& std : stds) {
        std = std::sqrt(std / static_cast<T>(numSamples));
    }
    
    // Standardize to mean=0, std=1
    for (auto& sample : data) {
        for (std::size_t i = 0; i < numFeatures; ++i) {
            if (stds[i] > T{0}) {
                sample[i] = (sample[i] - means[i]) / stds[i];
            }
        }
    }
}

template<typename T>
void DataLoader<T>::shuffle(Dataset<T>& dataset) {
    if (dataset.empty()) return;

    std::random_device rd;
    std::mt19937 gen(rd());

    // Create indices and shuffle them
    std::vector<std::size_t> indices(dataset.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), gen);

    // Create shuffled copies
    std::vector<std::vector<T>> shuffledInputs(dataset.size());
    std::vector<std::vector<T>> shuffledTargets(dataset.size());
    std::vector<std::string> shuffledLabels;

    if (!dataset.labels.empty()) {
        shuffledLabels.resize(dataset.size());
    }

    for (std::size_t i = 0; i < indices.size(); ++i) {
        shuffledInputs[i] = std::move(dataset.inputs[indices[i]]);
        shuffledTargets[i] = std::move(dataset.targets[indices[i]]);

        if (!dataset.labels.empty()) {
            shuffledLabels[i] = std::move(dataset.labels[indices[i]]);
        }
    }

    // Replace with shuffled data
    dataset.inputs = std::move(shuffledInputs);
    dataset.targets = std::move(shuffledTargets);

    if (!dataset.labels.empty()) {
        dataset.labels = std::move(shuffledLabels);
    }
}

template<typename T>
std::vector<std::vector<T>> DataLoader<T>::oneHotEncode(const std::vector<int>& labels, int numClasses) {
    if (labels.empty()) return {};

    // Determine number of classes if not specified
    if (numClasses <= 0) {
        numClasses = *std::max_element(labels.begin(), labels.end()) + 1;
    }

    std::vector<std::vector<T>> encoded(labels.size(), std::vector<T>(numClasses, T{0}));

    for (std::size_t i = 0; i < labels.size(); ++i) {
        int label = labels[i];
        if (label >= 0 && label < numClasses) {
            encoded[i][label] = T{1};
        }
    }

    return encoded;
}

template<typename T>
bool DataLoader<T>::saveToFile(const Dataset<T>& dataset,
                              const std::string& filename,
                              DataFormat format) {
    if (dataset.empty()) {
        NNV_LOG_WARNING("Cannot save empty dataset to file: {}", filename);
        return false;
    }

    try {
        if (format == DataFormat::CSV) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                NNV_LOG_ERROR("Failed to open file for writing: {}", filename);
                return false;
            }

            // Write each sample
            for (std::size_t i = 0; i < dataset.size(); ++i) {
                // Write input features
                for (const auto& feature : dataset.inputs[i]) {
                    file << feature << ",";
                }

                // Write target (assuming single target value)
                if (!dataset.targets[i].empty()) {
                    file << dataset.targets[i][0];
                }

                file << "\n";
            }

            NNV_LOG_INFO("Saved {} samples to CSV file: {}", dataset.size(), filename);
            return true;
        } else {
            NNV_LOG_ERROR("Unsupported format for saving dataset: {}",
                         static_cast<int>(format));
            return false;
        }

    } catch (const std::exception& e) {
        NNV_LOG_ERROR("Failed to save dataset to {}: {}", filename, e.what());
        return false;
    }
}

template<typename T>
std::vector<std::string> DataLoader<T>::getSupportedImageFormats() {
    return {".jpg", ".jpeg", ".png", ".bmp", ".tiff", ".tif"};
}

template<typename T>
DataFormat DataLoader<T>::detectFormat(const std::string& filename) {
    std::string ext = getFileExtension(filename);
    ext = toLower(ext);

    if (ext == ".csv") {
        return DataFormat::CSV;
    } else if (ext == ".json") {
        return DataFormat::JSON;
    } else if (ext == ".bin" || ext == ".dat") {
        return DataFormat::Binary;
    } else if (ext == ".idx3-ubyte" || ext == ".idx1-ubyte") {
        return DataFormat::MNIST;
    } else {
        auto supportedFormats = getSupportedImageFormats();
        if (std::find(supportedFormats.begin(), supportedFormats.end(), ext) !=
            supportedFormats.end()) {
            return DataFormat::Image;
        }
    }

    return DataFormat::CSV; // Default
}

template<typename T>
std::vector<std::string> DataLoader<T>::parseCSVLine(const std::string& line, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        // Trim whitespace
        token.erase(0, token.find_first_not_of(" \t\r\n"));
        token.erase(token.find_last_not_of(" \t\r\n") + 1);

        tokens.push_back(token);
    }

    return tokens;
}

template<typename T>
std::vector<std::vector<T>> DataLoader<T>::readMNISTImages(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open MNIST images file: " + filename);
    }

    // Read header
    uint32_t magic, numImages, numRows, numCols;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char*>(&numImages), sizeof(numImages));
    file.read(reinterpret_cast<char*>(&numRows), sizeof(numRows));
    file.read(reinterpret_cast<char*>(&numCols), sizeof(numCols));

    // Convert from big-endian to host endianness
    magic = __builtin_bswap32(magic);
    numImages = __builtin_bswap32(numImages);
    numRows = __builtin_bswap32(numRows);
    numCols = __builtin_bswap32(numCols);

    if (magic != 0x803) {
        throw std::runtime_error("Invalid MNIST images file format");
    }

    // Read image data
    std::vector<std::vector<T>> images(numImages, std::vector<T>(numRows * numCols));

    for (uint32_t i = 0; i < numImages; ++i) {
        for (uint32_t j = 0; j < numRows * numCols; ++j) {
            unsigned char pixel;
            file.read(reinterpret_cast<char*>(&pixel), 1);
            images[i][j] = static_cast<T>(pixel) / T{255};
        }
    }

    return images;
}

template<typename T>
std::vector<int> DataLoader<T>::readMNISTLabels(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open MNIST labels file: " + filename);
    }

    // Read header
    uint32_t magic, numLabels;
    file.read(reinterpret_cast<char*>(&magic), sizeof(magic));
    file.read(reinterpret_cast<char*>(&numLabels), sizeof(numLabels));

    // Convert from big-endian to host endianness
    magic = __builtin_bswap32(magic);
    numLabels = __builtin_bswap32(numLabels);

    if (magic != 0x801) {
        throw std::runtime_error("Invalid MNIST labels file format");
    }

    // Read label data
    std::vector<int> labels(numLabels);

    for (uint32_t i = 0; i < numLabels; ++i) {
        unsigned char label;
        file.read(reinterpret_cast<char*>(&label), 1);
        labels[i] = static_cast<int>(label);
    }

    return labels;
}

// Explicit template instantiations
template class Dataset<float>;
template class Dataset<double>;
template class DataLoader<float>;
template class DataLoader<double>;

} // namespace utils
} // namespace nnv
