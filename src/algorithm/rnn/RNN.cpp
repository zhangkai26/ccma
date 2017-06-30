/***********************************************
 * Author: Jun Jiang - jiangjun4@sina.com
 * Create: 2017-06-23 14:32
 * Last modified : 2017-06-23 14:32
 * Filename      : RNN.cpp
 * Description   : 
 **********************************************/

#include "algorithm/rnn/RNN.h"

namespace ccma{
namespace algorithm{
namespace rnn{

void RNN::sgd(std::vector<ccma::algebra::BaseMatrixT<real>*>* train_seq_data,
              std::vector<ccma::algebra::BaseMatrixT<real>*>* train_seq_label, 
              uint epoch, 
              real alpha){

    auto seq_data  = new ccma::algebra::DenseMatrixT<real>();
	auto seq_label = new ccma::algebra::DenseMatrixT<real>();
	
	uint num_train_data = train_seq_data->size();
	bool debug = (num_train_data <= 5);
    
	auto now = []{return std::chrono::system_clock::now();};

	for(uint i = 0; i != epoch; i++){
	
		auto start_time = now();
		
		for(uint j = 0; j != num_train_data; j++){
			train_seq_data->at(j)->clone(seq_data);
			train_seq_label->at(j)->clone(seq_label);

			if(seq_data->get_rows() != seq_label->get_rows()){
				printf("Data Error[%d][%d-%d].\n", j, seq_data->get_rows(), seq_label->get_rows());
				continue;
			}

			sgd_step(seq_data, seq_label, alpha, debug);

		//	if(j % 1 == 0){
        //        printf("Epoch[%d][%d/%d]training, loss[%f]...\n", i, j, num_train_data, loss(train_seq_data, train_seq_label));
		//	}
		}

        auto training_time = now();
        printf("Epoch[%d] training run time: %ld ms, loss[%f]\n", i, std::chrono::duration_cast<std::chrono::milliseconds>(training_time - start_time).count(), loss(train_seq_data, train_seq_label));
	}

	printf("training finished.\n");

    delete seq_data;
	delete seq_label;
}

void RNN::sgd_step(ccma::algebra::BaseMatrixT<real>* train_seq_data,
              ccma::algebra::BaseMatrixT<real>* train_seq_label, 
			  real alpha,
              bool debug){

    auto derivate_weight     = new ccma::algebra::DenseMatrixT<real>();
    auto derivate_pre_weight = new ccma::algebra::DenseMatrixT<real>();
    auto derivate_act_weight = new ccma::algebra::DenseMatrixT<real>();

	_layer->back_propagation(train_seq_data, train_seq_label, _U, _W, _V, derivate_weight, derivate_pre_weight, derivate_act_weight, debug);
	
	derivate_weight->multiply(alpha);
	_U->subtract(derivate_weight);

	derivate_pre_weight->multiply(alpha);
	_W->subtract(derivate_pre_weight);

	derivate_act_weight->multiply(alpha);
	_V->subtract(derivate_act_weight);

    delete derivate_weight;
    delete derivate_pre_weight;
    delete derivate_act_weight;
}

real RNN::total_loss(std::vector<ccma::algebra::BaseMatrixT<real>*>* train_seq_data,
                     std::vector<ccma::algebra::BaseMatrixT<real>*>* train_seq_label){

	real loss_value = 0;
	
	auto seq_data   = new ccma::algebra::DenseMatrixT<real>();
	auto seq_label  = new ccma::algebra::DenseMatrixT<real>();
	
    auto store      = new ccma::algebra::DenseMatrixT<real>();
	auto activation = new ccma::algebra::DenseMatrixT<real>();
	
    uint num_train_data = train_seq_data->size();
	for(uint j = 0; j != num_train_data; j++){

		train_seq_data->at(j)->clone(seq_data);
		train_seq_label->at(j)->clone(seq_label);

		_layer->feed_farward(seq_data, _U, _W, _V, store, activation, false);

		auto mat_label = seq_label->argmax(0);
        uint rows = mat_label->get_rows();

        for(uint row = 0; row != rows; row++){
            loss_value -= std::log(activation->get_data(row, mat_label->get_data(row, 0))); 
        }
        delete mat_label;

	}

    delete seq_data;
	delete seq_label;

    delete store;
    delete activation;

	return loss_value;
}

real RNN::loss(std::vector<ccma::algebra::BaseMatrixT<real>*>* train_seq_data,
               std::vector<ccma::algebra::BaseMatrixT<real>*>* train_seq_label){

    real loss_value = total_loss(train_seq_data, train_seq_label);
    uint N = 0;
    
    uint size = train_seq_label->size();
    for(uint i = 0; i != size; i++){
        N += train_seq_label->at(i)->get_rows();
    }

    return loss_value / N;
}

}//namespace rnn
}//namespace algorithm
}//namespace ccma
