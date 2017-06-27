/*********************************************
* Author: Jun Jiang - jiangjun4@sina.com
* Created: 2017-06-01 11:22
* Last modified: 2017-06-01 11:22
* Filename: TestRNN.cpp
* Description: 
**********************************************/

#include "utils/RNNHelper.h"
#include "algorithm/rnn/RNN.h"

int main(int argc, char** argv){
	std::map<std::string, uint> map;
	ccma::utils::RNNHelper helper(8000);
	helper.read_word2index("data/word_to_index", map);

	std::vector<ccma::algebra::BaseMatrixT<real>*> data_seq_data;
	std::vector<ccma::algebra::BaseMatrixT<real>*> data_seq_label;

	helper.read_seqdata("data/train_seq_data", &data_seq_data, "data/train_seq_label", &data_seq_label, 1000);

    printf("Start training....\n");

    auto rnn = new ccma::algorithm::rnn::RNN(8000, 100);
    rnn->sgd(&data_seq_data, &data_seq_label);

    delete rnn;
}