/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2016-12-07 15:14
* Last modified: 2016-12-07 15:14
* Filename: LinearRegress.h
* Description:linear regression
**********************************************/
#ifndef _CCMA_ALGORITHM_REGRESSION_H_
#define _CCMA_ALGORITHM_REGRESSION_H_

#include "algebra/BaseMatrix.h"

namespace ccma{
namespace algorithm{
namespace regression{

template<class T, class LT, class FT>
class LinearRegression{
public:
    explicit LinearRegression(ccma::algebra::LabeledMatrixT<T, LT, FT>* train_data);

    int standard_regression();

private:
    ccma::algebra::LabeledMatrixT<T, LT, FT>* _train_data = nullptr;

    ccma::algebra::ColMatrixT<T>* _weights = nullptr;

};//class LinearRegression


template<class T, class LT, class FT>
LinearRegression<T, LT, FT>::LinearRegression(ccma::algebra::LabeledMatrixT<T, LT, FT>* train_data){
    _train_data = train_data;
    _weight = new ccma::algebra::ColMatrix<real>(train_data->get_cols(), (real)0);
}

template<class T, class LT, class FT>
int LinearRegression<T, LT, FT>::standard_regression(){
    ccma::algebra::BaseMatrixT<T>* x = new ccma::algebra::BaseMatrixT<T>();
    _train_data->copy_data(x);

    ccma::algebra::BaseMatrixT<LT>* y = new ccma::algebra::BaseMatrixT<LT>();
    _train_data->get_labels(y);

    ccma::algebra::BaseMatrixT<T>* xT = new ccma::algebra::BaseMatrixT<T>();
    x->transpose(xT);

    ccma::algebra::BaseMatrixT<T>* xTx = new ccma::algebra::BaseMatrixT<T>();
    xT->dot_product(x, xTx);

    ccma::algebra::BaseMatrixT<T>* xTxI = new ccma::algebra::BaseMatrixT<T>();
    if(xTx->inverse(xTxI) < 0){
        return -1;
    }

    ccma::algebra::BaseMatrixT<T>* xTy = new ccma::algebra::BaseMatrixT<T>();
    xT->dot_product(y, xTy);

    xTxI->dot_product(xTy, _weights);

    _weights->display();
}

}//regression
}//namespace
}//namespace ccma

#endif //_CCMA_ALGORITHM_REGRESSION_H_
