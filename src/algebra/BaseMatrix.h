/**********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-11-22 11:30
* Last modified: 2016-11-22 11:30
* Filename: BaseMatrix.h
* Description: Base Data Structure of training/predict data
**********************************************/

#ifndef _CCMA_ALGEBRA_BASEMATRIX_H_
#define _CCMA_ALGEBRA_BASEMATRIX_H_

#include <cmath>
#include <string.h>
#include <limits.h>
#include <unordered_map>
#include "utils/TypeDef.h"

namespace ccma{
namespace algebra{

template<class T>
class BaseMatrixT{
public:
    BaseMatrixT(){}
    BaseMatrixT(const uint rows, const uint cols){
        _rows = rows;
        _cols = cols;
    }

    virtual ~BaseMatrixT() = default;

    inline uint get_rows() const { return this->_rows;}
    inline uint get_cols() const { return this->_cols;}

    virtual BaseMatrixT<T>* copy_data() = 0;

    virtual T* get_data() const = 0;
    virtual void set_data(const T* data,
                          const uint rows,
                          const uint cols) = 0;
    virtual void set_shallow_data(T* data,
                                  const uint rows,
                                  const uint cols) = 0;

    virtual T get_data(const uint idx) const = 0;
    virtual bool set_data(const T& value, const uint idx) = 0;

    virtual T get_data(const uint row, const uint col) const = 0;
    virtual bool set_data(const T& data,
                          const uint row,
                          const uint col) = 0;

    virtual BaseMatrixT<T>* get_row_data(const uint row) const = 0;
    virtual bool set_row_data(BaseMatrixT<T>* mat, const int row) = 0;

    virtual bool extend(const BaseMatrixT<T>* mat) = 0;

    virtual bool add(const BaseMatrixT<T>* mat) = 0;
    virtual bool add(const BaseMatrixT<T>* mat, BaseMatrixT<T>* result) = 0;

    virtual bool subtract(const BaseMatrixT<T>* mat) = 0;
    virtual bool subtract(const BaseMatrixT<T>* mat, BaseMatrixT<T>* result) = 0;


    virtual bool product(const T value) = 0;
    virtual bool product(const T value, BaseMatrixT<T>* result) = 0;
    virtual bool product(const BaseMatrixT<T>* mat, BaseMatrixT<T>* result) = 0;
    virtual bool product(const BaseMatrixT<int>* mat, BaseMatrixT<real>* result) = 0;

    virtual bool swap(const uint a_row,
                      const uint a_col,
                      const uint b_row,
                      const uint b_col) = 0;
    virtual bool swap_row(const uint a, const uint b) = 0;
    virtual bool swap_col(const uint a, const uint b) = 0;

    virtual void transpose() = 0;
    virtual void transpose(BaseMatrixT<T>* result) = 0;

    virtual bool det(T* result) = 0;

    virtual bool inverse(BaseMatrixT<real>* result) = 0;

    virtual void display() = 0;
protected:
    uint _rows;
    uint _cols;

};//class BaseMatrixT


template<class T>
class DenseMatrixT : public BaseMatrixT<T>{
public:
    DenseMatrixT();
    DenseMatrixT(const T* data,
                 const uint rows,
                 const uint cols);
    ~DenseMatrixT();

    DenseMatrixT<T>* copy_data();

    T* get_data() const;
    void set_data(const T* data,
                  const uint rows,
                  const uint cols);

    T get_data(const uint idx) const;
    bool set_data(const T& value, const uint idx);

    T get_data(const uint row, const uint col) const;
    bool set_data(const T& data,
                  const uint row,
                  const uint col);
    void set_shallow_data(T* data,
                          const uint rows,
                          const uint cols);


    DenseMatrixT<T>* get_row_data(const uint row) const;
    bool set_row_data(BaseMatrixT<T>* mat, const int row);

    bool extend(const BaseMatrixT<T>* mat);

    bool add(const BaseMatrixT<T>* mat);
    bool add(const BaseMatrixT<T>* mat, BaseMatrixT<T>* result);

    bool subtract(const BaseMatrixT<T>* mat);
    bool subtract(const BaseMatrixT<T>* mat, BaseMatrixT<T>* result);


    bool product(const T value);
    bool product(const T value, BaseMatrixT<T>* result);
    bool product(const BaseMatrixT<T>* mat, BaseMatrixT<T>* result);
    bool product(const BaseMatrixT<int>* mat, BaseMatrixT<real>* result);

    bool swap(const uint a_row,
              const uint a_col,
              const uint b_row,
              const uint b_col);
    bool swap_row(const uint a, const uint b);
    bool swap_col(const uint a, const uint b);

    void transpose();
    void transpose(BaseMatrixT<T>* result);

    bool det(T* result);

    bool inverse(BaseMatrixT<real>* result);

    void display();
protected:
    T* _data;

    inline bool check_range(const uint idx){
        return idx < this->_rows * this->_cols;
    }
    inline bool check_range(const uint row, const uint col){
        return row < this->_rows && col < this->_cols;
    }

    inline void clear_cache(){
        this->_cache_matrix_det = static_cast<T>(INT_MAX);
    }

private:
    T _cache_matrix_det;
};


template<class T>
class DenseMatrixMNT : public DenseMatrixT<T>{
public:
    DenseMatrixMNT(const T* data, uint rows, uint cols):DenseMatrixT<T>(data, rows, cols){}

    DenseMatrixMNT(uint rows, uint cols, T value){
        this->_data = new T[rows * cols];

        //memset only support 0 or -1.
        if(value == static_cast<T>(0) || value == static_cast<T>(-1)){
            memset(this->_data, value, sizeof(T) * rows * cols);
        }else{
            for(uint i = 0; i < rows * cols; i++){
                this->_data[i] = value;
            }
        }
        this->_rows = rows;
        this->_cols = cols;
    }
};

template<class T>
class DenseColMatrixT : public DenseMatrixMNT<T>{
public:
    DenseColMatrixT(uint rows, T value):DenseMatrixMNT<T>(rows, 1, value){}
    DenseColMatrixT(const T* data, uint rows):DenseMatrixMNT<T>(data, rows, 1){}
};

template<class T>
class DenseRowMatrixT : public DenseMatrixMNT<T>{
public:
    DenseRowMatrixT(uint cols, T value):DenseMatrixMNT<T>(1, cols, value){}
    DenseRowMatrixT(const T* data, uint cols):DenseMatrixMNT<T>(data, 1, cols){}
};

template<class T>
class DenseOneMatrixT : public DenseMatrixMNT<T>{
public:
    explicit DenseOneMatrixT(uint rows):DenseMatrixMNT<T>(rows, 1, 1){}
};

template<class T>
class DenseZeroMatrixT : public DenseMatrixMNT<T>{
public:
    explicit DenseZeroMatrixT(uint rows):DenseMatrixMNT<T>(rows, 1, 0){}
};

template<class T>
class DenseEyeMatrixT : public DenseMatrixT<T>{
public:
    explicit DenseEyeMatrixT(uint size){

        T* data = new T[size * size];
        memset(data, 0, sizeof(T) * size * size);

        for(uint i = 0; i < size; i++){
            data[i * size + i] = static_cast<T>(1);
        }

        this->set_shallow_data(data, size, size);
    }
};


template<class T>
class CCMap : public std::unordered_map<T, uint>{
public:
    CCMap() : std::unordered_map<T, uint>(), _value(0){}

    uint get_max_value(){
        if(_value == 0){
            find_max_iterator();
        }
        return _value;
    }

    T get_max_key(){
        if(_value == 0){
            find_max_iterator();
        }
        return _key;
    }

private:
    uint _value;
    T _key;

    void find_max_iterator(){
        typename std::unordered_map<T, uint>::const_iterator it;
        for(it = this->begin(); it != this->end(); it++){
            if(it->second > _value){
                _value = it->second;
                _key = it->first;
            }
        }
    }

};


template<class T>
class LabeledDenseMatrixT : public DenseMatrixT<T>{
public:
    LabeledDenseMatrixT();
    LabeledDenseMatrixT(const T* data,
                        const T* labels,
                        const uint rows,
                        const uint cols);

    LabeledDenseMatrixT(const T* data,
                        const T* labels,
                        const uint* feature_names,
                        const uint rows,
                        const uint cols);
    ~LabeledDenseMatrixT();

    inline T get_label(uint idx) const{
        return this->_labels[idx];
    }

    inline uint get_feature_name(uint idx) const{
        return this->_feature_names[idx];
    }

    DenseColMatrixT<T>* get_labels();

    void set_data(const T* data,
                  const T* labels,
                  const uint rows,
                  const uint cols);

    void set_data(const T* data,
                  const T* labels,
                  const uint* feature_names,
                  const uint rows,
                  const uint cols);

    void set_shallow_data(T* data,
                          T* labels,
                          const uint rows,
                          const uint cols);

    void set_shallow_data(T* data,
                          T* labels,
                          uint* feature_names,
                          const uint rows,
                          const uint cols);

    real get_shannon_entropy();

    bool split(uint feature_idx,
               T split_value,
               LabeledDenseMatrixT<T>* sub_mat);

    CCMap<T>* get_label_cnt_map();

    CCMap<T>* get_feature_cnt_map(uint feature_idx);

    void display();

protected:
    T* _labels = nullptr;
    uint* _feature_names = nullptr;

    void clear_cache();

private:
    real _cache_shannon_entropy = 0.0;
    CCMap<T>* _cache_label_cnt_map;
    std::unordered_map<uint, CCMap<T>* >* _cache_feature_cnt_map;
};


}//namespace algebra
}//namespace ccma

#endif //_CCMA_ALGEBRA_BASEMATRIX_H_
