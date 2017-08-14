#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "seal.h"
#include <time.h>
#include <sys/time.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sstream>
using namespace std;
using namespace seal;

//1
void read_csv(string csv_file_name,
	      vector < vector<float> > & csv_data){
  ifstream csv(csv_file_name);
  string line;
  string token;
  vector <float> temp;
  int k = 0;
  if (csv.is_open()) {
    while(csv.good()){
      getline(csv, line);
      istringstream buffer (line);
      while( getline( buffer, token, ',' ) ){
	        temp.push_back(strtof((token).c_str(),0));
      }
      csv_data.push_back(temp);
      temp.clear();
    }
  }
}
//2
void encrypt_data(vector<vector<float>> &csv_data,
		  vector<vector<Ciphertext>> &data_enc, Encryptor encryptor, Decryptor decryptor, Evaluator evaluator,FractionalEncoder encoder){
   vector<Ciphertext> encrypted_temp;
  for (int x = 0; x < csv_data.size(); ++x){
            for (int y = 0; y < csv_data[x].size(); ++y){
        	  Plaintext encoded_number = encoder.encode(csv_data[x][y]);
	           encrypted_temp.emplace_back(encryptor.encrypt(encoded_number));
	     }
             data_enc.push_back(encrypted_temp);
             encrypted_temp.clear();
    }
}
/*
//7
void train(const vector<vector<Ciphertext>> &data_enc,
	   vector<vector<Ciphertext>> &model_enc){
}
//5
void erreur(vector<vector<Ciphertext>> &model_enc,
	    float &loss){
}
//8
void predict(vector<vector<Ciphertext>> &model_enc,
	     Ciphertext &result_enc){
}

//6
void SGD(const vector<Ciphertext> &sample_enc,
	 vector<Ciphertext> &model_enc,
	 Plaintext &lr){
	 }*/
//4
Ciphertext sigmoid_enc(vector<Ciphertext>& X_enc, vector<Ciphertext>& model_enc, Encryptor encryptor, Decryptor decryptor, Evaluator evaluator,FractionalEncoder encoder, Evaluator evaluator2){
  
  X_enc.insert( X_enc.begin(), encryptor.encrypt(encoder.encode(1)));
  vector<Ciphertext> vect_somme;
  Plaintext taylor_constant = encoder.encode(1.0/2);
  vector<Plaintext> taylor_coeffs{encoder.encode(1.0/4),encoder.encode(-1.0/48), encoder.encode(1.0/480), encoder.encode(-17.0/80640)};
 
  for(int i=0 ; i< X_enc.size()-1 ; i++){
         Ciphertext mult = evaluator.multiply(X_enc[i], model_enc[i]);
         vect_somme.push_back(mult);
    }
  Ciphertext somme_negate = evaluator.negate(evaluator.add_many(vect_somme));
  Ciphertext T = evaluator2.relinearize(somme_negate);
  Ciphertext result = evaluator.multiply_plain(T , taylor_coeffs[0]);
  vector<Ciphertext> vect_puissance;
  for(int j = 1; j < 4; ++j){
        vect_puissance.push_back(evaluator.exponentiate(T, 2*j+1));
        Ciphertext prod = evaluator.multiply_plain(vect_puissance[j-1], taylor_coeffs[j]);
        result = evaluator.add(result, prod);
	}
  Plaintext test2 = decryptor.decrypt(result);
  float     test_2 = encoder.decode(test2);
  cout<<"test_2 = "<<test_2<<endl;
   
  return result;
  }
void model_encryption(vector<float>& model,
		      vector<Ciphertext>& model_enc, Encryptor encryptor, Decryptor decryptor, Evaluator evaluator,FractionalEncoder encoder){
                      for (int y = 0; y < model.size(); ++y){
                           Plaintext encoded_number = encoder.encode(model[y]);
                           model_enc.emplace_back(encryptor.encrypt(encoded_number));
                         }
}

		      
//3
/*vector<Ciphertext> der_sigmoid_enc(vector<Ciphertext> &X_enc){
}*/
int main (){
  EncryptionParameters parms;
  parms.set_poly_modulus("1x^4096 + 1");
  parms.set_coeff_modulus(ChooserEvaluator::default_parameter_options().at(4096));
  parms.set_plain_modulus(1 << 4);
  parms.set_decomposition_bit_count(16);
  parms.validate();
  KeyGenerator generator(parms);
  generator.generate(1);
  Ciphertext public_key = generator.public_key();
  Plaintext secret_key = generator.secret_key();
   EvaluationKeys evaluation_keys = generator.evaluation_keys();
  FractionalEncoder encoder(parms.plain_modulus(), parms.poly_modulus(), 64, 32, 3);
  Encryptor encryptor(parms, public_key);
  Decryptor decryptor(parms, secret_key);
  Evaluator evaluator(parms, evaluation_keys);
  vector<vector<float> > csv_data;
  vector<float> model{1,1,1,1,1,1,1,1,1};
  vector<Ciphertext> model_enc;
  vector< vector<Ciphertext> > data_enc;
  string csv_file_name = "pima.csv";
  read_csv(csv_file_name,csv_data);
  encrypt_data(csv_data, data_enc, encryptor, decryptor, evaluator, encoder);
  model_encryption(model, model_enc,encryptor, decryptor, evaluator, encoder);
  cout <<"-------------------------------"<<endl;
  generator.generate_evaluation_keys(1);
  //EvaluationKeys evaluation_keys = generator.evaluation_keys();
  Evaluator evaluator2(parms, evaluation_keys);
  Ciphertext result = sigmoid_enc (data_enc[1], model_enc,encryptor, decryptor, evaluator, encoder, evaluator2);
  Plaintext test2 = decryptor.decrypt(result);
  float     test_2 = encoder.decode(test2);
  cout<<"test_2 = "<<test_2<<endl;
  
  return 0;
}


  
