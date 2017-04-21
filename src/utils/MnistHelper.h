/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2017-04-18 17:41
* Last modified: 2017-04-18 17:41
* Filename: MnistHelper.h
* Description: read mnist file
**********************************************/

#ifndef _CCMA_UTILS_MNISTHELPER_H_
#define _CCMA_UTILS_MNISTHELPER_H_

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include "algebra/BaseMatrix.h"

namespace ccma{
namespace utils{

template<class T>
class MnistHelper{
public:
    ccma::algebra::LabeledDenseMatrixT<T>* read(const std::string& image_file,
                                                const std::string& label_file,
                                                const int limit = -1,
                                                const uint threshold = 30);

    inline T* vectorize_label(const uint data, const uint sizes);

    ccma::algebra::BaseMatrixT<T>* read_image(const std::string& image_file,
                                        const int limit = -1,
                                        const uint threshold = 30);

    ccma::algebra::BaseMatrixT<T>* read_label(const std::string& label_file,
                                              const int limit = -1,
                                              const uint vec_size = 10);
private:
    inline std::unique_ptr<char[]> read_mnist_file(const std::string& path,
                                                   uint key,
                                                   uint* out_rows);
    inline uint read_header(const std::unique_ptr<char[]>& buffer, size_t position);

};//class MnistHelper

template<class T>
ccma::algebra::LabeledDenseMatrixT<T>* MnistHelper<T>::read(const std::string& image_file,
                                                            const std::string& label_file,
                                                            const int limit,
                                                            const uint threshold){

    ccma::algebra::LabeledDenseMatrixT<T>* mat;

    uint image_num = 0;
    uint label_num = 0;
    auto image_buffer = read_mnist_file(image_file, 0x803, &image_num);
    auto label_buffer = read_mnist_file(label_file, 0x801, &label_num);
    if(!image_buffer || !label_buffer || image_num != label_num){
        mat = new ccma::algebra::LabeledDenseMatrixT<T>();
        return mat;
    }

    auto count = image_num;
    if( limit > 0 && limit < count){
        count = limit;
    }

    //read image data
    auto rows = read_header(image_buffer, 2);
    auto cols = read_header(image_buffer, 3);

    auto image_data_buffer = reinterpret_cast<unsigned char*>(image_buffer.get() + 16);

    T* data = new T[count * rows * cols];
    for(size_t i = 0; i < count * rows * cols; i++){
        auto pixel = *image_data_buffer++;
        pixel = (pixel >= threshold) ? 1 : 0;
        if(pixel != 0){
            printf("%d\t%d\n", pixel, i);
        }
        data[i] = static_cast<T>(pixel);
    }

    //read label data
    auto label_data_buffer = reinterpret_cast<unsigned char*>(label_buffer.get() + 8);

    T* labels = new T[count];
    for(size_t i = 0; i < count; i++){
        auto label  = *label_data_buffer++;
        labels[i]   = static_cast<T>(label);
    }

    mat = new ccma::algebra::LabeledDenseMatrixT<T>(data, labels, count, rows * cols);
    return mat;
}

template<class T>
ccma::algebra::BaseMatrixT<T>* MnistHelper<T>::read_image(const std::string& image_file,
                                                          const int limit,
                                                          const uint threshold){
    auto mat = new ccma::algebra::DenseMatrixT<T>();
    uint count = 0;
    auto image_buffer = read_mnist_file(image_file, 0x803, &count);

    if(!image_buffer || count == 0){
        return mat;
    }

    if( limit > 0 && limit < count){
        count = limit;
    }

    //read image data
    auto rows = read_header(image_buffer, 2);
    auto cols = read_header(image_buffer, 3);

    auto image_data_buffer = reinterpret_cast<unsigned char*>(image_buffer.get() + 16);

    T* data = new T[count * rows * cols];
    for(size_t i = 0; i < count * rows * cols; i++){
        data[i] = static_cast<T>((*image_data_buffer++ >= threshold) ? 1 : 0);
    }

    mat->set_shallow_data(data, count, rows * cols);

    return mat;
}

template<class T>
ccma::algebra::BaseMatrixT<T>* MnistHelper<T>::read_label(const std::string& label_file,
                                                          const int limit,
                                                          const uint vec_size){
    auto mat = new ccma::algebra::DenseMatrixT<T>();
    uint count = 0;
    auto label_buffer = read_mnist_file(label_file, 0x801, &count);
    if(!label_buffer || count == 0){
        return mat;
    }

    if( limit > 0 && limit < count){
        count = limit;
    }

    //read label data
    auto label_data_buffer = reinterpret_cast<unsigned char*>(label_buffer.get() + 8);

    T* labels = new T[count * vec_size];
    memset(labels, 0, sizeof(T) * count * vec_size);

    for(size_t i = 0; i < count; i++){
        auto label = *label_data_buffer++;
        labels[i * vec_size + label] = 1;
    }

    mat->set_shallow_data(labels, count, vec_size);

    return mat;
}

template<class T>
inline std::unique_ptr<char[]> MnistHelper<T>::read_mnist_file(const std::string& path,
                                                            uint key,
                                                            uint* out_rows){

    *out_rows = 0;

    std::ifstream file;
    file.open(path, std::ios::in | std::ios::binary | std::ios::ate);

    if(!file){
        std::cout << "Can't open file:" << path << std::endl;
        return {};
    }

    auto size = file.tellg();
    std::unique_ptr<char[]> buffer(new char[size]);

    //read the entire file to buffer
    file.seekg(0, std::ios::beg);
    file.read(buffer.get(), size);
    file.close();

    auto magic_num = read_header(buffer, 0);

    if(magic_num != key){
        std::cout << "Invalid magic number, probably not  a MNIST file" << std::endl;
        return {};
    }

    auto count = read_header(buffer, 1);

    if(magic_num == 0x803){
        auto rows = read_header(buffer, 2);
        auto cols = read_header(buffer, 3);

        if(size < count * rows * cols + 16){
            std::cout << "File size ERROR" << std::endl;
            return {};
        }
    }else if( magic_num == 0x801){
        if(size < count + 8){
            std::cout << "File size ERROR" << std::endl;
            return {};
        }
    }

    *out_rows = count;

    return std::move(buffer);
}

template<class T>
inline uint MnistHelper<T>::read_header(const std::unique_ptr<char[]>& buffer, size_t position){
    auto header = reinterpret_cast<uint*>(buffer.get());

    auto value = *(header + position);
    return (value << 24) | ((value << 8) & 0x00FF0000) | ((value >> 8) & 0X0000FF00) | (value >> 24);
}

template<class T>
inline T* MnistHelper<T>::vectorize_label(const uint data, const uint sizes){
    T* vec = new T[sizes];
    memset(vec, static_cast<T>(0), sizeof(T) * sizes);

    if(data < sizes){
        vec[data] = static_cast<T>(1);
    }

    return vec;
}

}//namespace utils
}//namespace ccma
#endif