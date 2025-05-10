/**
 * @file DataLoader.hpp
 * @brief Data loading and preprocessing utilities
 * @author Neural Network Visualizer Team
 * @version 1.0.0
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <unordered_map>

#include "core/Types.hpp"
#include "utils/Common.hpp"

namespace nnv {
namespace utils {

/**
 * @brief Data format types
 */
enum class DataFormat {
    CSV,
    MNIST,
    Image,
    Binary,
    JSON
};

/**
 * @brief Data preprocessing options
 */
struct PreprocessingConfig {
    bool normalize = true;          ///< Normalize to [0, 1]
    bool standardize = false;       ///< Standardize to mean=0, std=1
    bool shuffle = true;            ///< Shuffle data
    float validationSplit = 0.0f;   ///< Validation split ratio
    std::pair<int, int> imageSize = {28, 28}; ///< Target image size
    bool grayscale = true;          ///< Convert to grayscale
};

/**
 * @brief Data augmentation options
 */
struct AugmentationConfig {
    bool enabled = false;
    float rotationRange = 0.0f;     ///< Rotation range in degrees
    float scaleRange = 0.0f;        ///< Scale range (0.1 = ±10%)
    float translationRange = 0.0f;  ///< Translation range (0.1 = ±10%)
    float brightnessRange = 0.0f;   ///< Brightness range (0.1 = ±10%)
    float contrastRange = 0.0f;     ///< Contrast range (0.1 = ±10%)
    float noiseLevel = 0.0f;        ///< Gaussian noise level
};

/**
 * @brief Dataset structure
 */
template<typename T = core::Scalar>
struct Dataset {
    std::vector<std::vector<T>> inputs;     ///< Input data
    std::vector<std::vector<T>> targets;    ///< Target data
    std::vector<std::string> labels;        ///< Class labels
    std::unordered_map<std::string, int> labelMap; ///< Label to index mapping
    
    /**
     * @brief Get dataset size
     * @return Number of samples
     */
    std::size_t size() const { return inputs.size(); }
    
    /**
     * @brief Check if dataset is empty
     * @return True if empty
     */
    bool empty() const { return inputs.empty(); }
    
    /**
     * @brief Clear dataset
     */
    void clear() {
        inputs.clear();
        targets.clear();
        labels.clear();
        labelMap.clear();
    }
    
    /**
     * @brief Split dataset into train/validation
     * @param validationRatio Validation split ratio
     * @return Pair of (train, validation) datasets
     */
    std::pair<Dataset<T>, Dataset<T>> split(float validationRatio) const;
};

/**
 * @brief Main data loader class
 */
template<typename T = core::Scalar>
class DataLoader {
public:
    /**
     * @brief Constructor
     */
    DataLoader() = default;
    
    /**
     * @brief Destructor
     */
    ~DataLoader() = default;
    
    /**
     * @brief Load data from file
     * @param filename File path
     * @param format Data format (auto-detected if not specified)
     * @param config Preprocessing configuration
     * @return Loaded dataset
     */
    Dataset<T> loadFromFile(const std::string& filename, 
                           DataFormat format = DataFormat::CSV,
                           const PreprocessingConfig& config = {});
    
    /**
     * @brief Load CSV data
     * @param filename CSV file path
     * @param hasHeader Whether CSV has header row
     * @param delimiter Column delimiter
     * @param targetColumn Target column index (-1 for last column)
     * @return Loaded dataset
     */
    Dataset<T> loadCSV(const std::string& filename,
                      bool hasHeader = true,
                      char delimiter = ',',
                      int targetColumn = -1);
    
    /**
     * @brief Load MNIST data
     * @param imagesFile MNIST images file path
     * @param labelsFile MNIST labels file path
     * @return Loaded dataset
     */
    Dataset<T> loadMNIST(const std::string& imagesFile,
                        const std::string& labelsFile);
    
    /**
     * @brief Load images from directory
     * @param directory Directory path
     * @param config Preprocessing configuration
     * @return Loaded dataset
     */
    Dataset<T> loadImagesFromDirectory(const std::string& directory,
                                      const PreprocessingConfig& config = {});
    
    /**
     * @brief Load single image
     * @param filename Image file path
     * @param config Preprocessing configuration
     * @return Image data as vector
     */
    std::vector<T> loadImage(const std::string& filename,
                            const PreprocessingConfig& config = {});
    
    /**
     * @brief Apply preprocessing to dataset
     * @param dataset Dataset to preprocess
     * @param config Preprocessing configuration
     */
    void preprocess(Dataset<T>& dataset, const PreprocessingConfig& config);
    
    /**
     * @brief Apply data augmentation
     * @param dataset Dataset to augment
     * @param config Augmentation configuration
     * @param multiplier Number of augmented samples per original sample
     */
    void augment(Dataset<T>& dataset, 
                const AugmentationConfig& config,
                int multiplier = 1);
    
    /**
     * @brief Normalize data to [0, 1] range
     * @param data Data to normalize
     */
    void normalize(std::vector<std::vector<T>>& data);
    
    /**
     * @brief Standardize data to mean=0, std=1
     * @param data Data to standardize
     */
    void standardize(std::vector<std::vector<T>>& data);
    
    /**
     * @brief Shuffle dataset
     * @param dataset Dataset to shuffle
     */
    void shuffle(Dataset<T>& dataset);
    
    /**
     * @brief Convert labels to one-hot encoding
     * @param labels Class labels
     * @param numClasses Number of classes (auto-detected if 0)
     * @return One-hot encoded targets
     */
    std::vector<std::vector<T>> oneHotEncode(const std::vector<int>& labels,
                                            int numClasses = 0);
    
    /**
     * @brief Save dataset to file
     * @param dataset Dataset to save
     * @param filename Output file path
     * @param format Output format
     * @return True if successful
     */
    bool saveToFile(const Dataset<T>& dataset,
                   const std::string& filename,
                   DataFormat format = DataFormat::CSV);
    
    /**
     * @brief Get supported image formats
     * @return Vector of supported extensions
     */
    static std::vector<std::string> getSupportedImageFormats();
    
    /**
     * @brief Detect data format from file extension
     * @param filename File path
     * @return Detected format
     */
    static DataFormat detectFormat(const std::string& filename);

private:
    /**
     * @brief Parse CSV line
     * @param line CSV line
     * @param delimiter Column delimiter
     * @return Vector of values
     */
    std::vector<std::string> parseCSVLine(const std::string& line, char delimiter);
    
    /**
     * @brief Read MNIST images file
     * @param filename Images file path
     * @return Vector of image data
     */
    std::vector<std::vector<T>> readMNISTImages(const std::string& filename);
    
    /**
     * @brief Read MNIST labels file
     * @param filename Labels file path
     * @return Vector of labels
     */
    std::vector<int> readMNISTLabels(const std::string& filename);
    
    /**
     * @brief Apply single augmentation
     * @param image Input image
     * @param config Augmentation configuration
     * @param imageSize Image dimensions
     * @return Augmented image
     */
    std::vector<T> applySingleAugmentation(const std::vector<T>& image,
                                          const AugmentationConfig& config,
                                          std::pair<int, int> imageSize);
    
    /**
     * @brief Rotate image
     * @param image Input image
     * @param angle Rotation angle in degrees
     * @param imageSize Image dimensions
     * @return Rotated image
     */
    std::vector<T> rotateImage(const std::vector<T>& image,
                              float angle,
                              std::pair<int, int> imageSize);
    
    /**
     * @brief Scale image
     * @param image Input image
     * @param scale Scale factor
     * @param imageSize Image dimensions
     * @return Scaled image
     */
    std::vector<T> scaleImage(const std::vector<T>& image,
                             float scale,
                             std::pair<int, int> imageSize);
    
    /**
     * @brief Translate image
     * @param image Input image
     * @param dx X translation
     * @param dy Y translation
     * @param imageSize Image dimensions
     * @return Translated image
     */
    std::vector<T> translateImage(const std::vector<T>& image,
                                 int dx, int dy,
                                 std::pair<int, int> imageSize);
    
    /**
     * @brief Adjust brightness
     * @param image Input image
     * @param factor Brightness factor
     * @return Brightness-adjusted image
     */
    std::vector<T> adjustBrightness(const std::vector<T>& image, float factor);
    
    /**
     * @brief Adjust contrast
     * @param image Input image
     * @param factor Contrast factor
     * @return Contrast-adjusted image
     */
    std::vector<T> adjustContrast(const std::vector<T>& image, float factor);
    
    /**
     * @brief Add noise to image
     * @param image Input image
     * @param noiseLevel Noise level
     * @return Noisy image
     */
    std::vector<T> addNoise(const std::vector<T>& image, float noiseLevel);
};

// Type aliases
using FloatDataLoader = DataLoader<float>;
using DoubleDataLoader = DataLoader<double>;
using FloatDataset = Dataset<float>;
using DoubleDataset = Dataset<double>;

} // namespace utils
} // namespace nnv
