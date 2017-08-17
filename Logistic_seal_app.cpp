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

Ciphertext sigmoid_enc( vector<Ciphertext>& X_enc, vector<Ciphertext>& model_enc, Encryptor encryptor, Decryptor decryptor, Evaluator evaluator,FractionalEncoder encoder){

  X_enc.insert( X_enc.begin(), encryptor.encrypt(encoder.encode(1)));
  vector<Ciphertext> vect_somme;
  for(int i=0 ; i< X_enc.size()-1 ; i++){
    Ciphertext mult = evaluator.multiply(X_enc[i], model_enc[i]);
    vect_somme.push_back(mult);
  }
  Ciphertext somme = evaluator.add_many(vect_somme);
  Ciphertext somme_div = evaluator.multiply_plain(somme , encoder.encode(1.0/2) );
  Ciphertext add_one = evaluator.add_plain(somme_div, encoder.encode(1));
  Ciphertext add_one_demi = evaluator.multiply_plain(add_one, encoder.encode(1.0/2));
  vect_somme.clear();
  if(encoder.decode(decryptor.decrypt(somme)) < -2)
   return encryptor.encrypt(encoder.encode(0));
  else if (encoder.decode(decryptor.decrypt(somme)) > -2 && encoder.decode(decryptor.decrypt(somme)) < 2)
   return add_one_demi;
  else
     return encryptor.encrypt(encoder.encode(1));

}
void SGD(vector<Ciphertext> &sample_enc,
	 vector<Ciphertext> &model_enc,Plaintext &lr, Encryptor encryptor, Decryptor decryptor, Evaluator evaluator,FractionalEncoder encoder){
 
    Ciphertext sigmoid = sigmoid_enc(sample_enc, model_enc, encryptor, decryptor, evaluator, encoder);
    Ciphertext sigmoid_class = evaluator.sub(sigmoid, sample_enc[sample_enc.size()-1]);
    Ciphertext sigmoid_class_lr = evaluator.multiply_plain(sigmoid_class, lr);
    model_enc[0] = evaluator.sub(model_enc[0], sigmoid_class_lr);
    for( int i = 1; i < model_enc.size(); i++){
           Ciphertext sigmoid_class_mult = evaluator.multiply(sigmoid_class_lr, sample_enc[i]);
           model_enc[i] = evaluator.sub(model_enc[i], sigmoid_class_mult );
	   
	    }
}
void train(vector<vector<Ciphertext>>& data_enc,                                                                                                           
	   vector<Ciphertext>& model_enc, Encryptor encryptor, Decryptor decryptor, Evaluator evaluator,FractionalEncoder encoder){
  
  Plaintext encoded_lr = encoder.encode(0.001);
  for(int i = 0; i<data_enc.size(); ++i){
    for(int j=0; j<model_enc.size();++j)
      cout<<"theta ["<<j<<"] = "<<encoder.decode(decryptor.decrypt(model_enc[j]))<<endl;
    
      SGD(data_enc[i], model_enc, encoded_lr, encryptor, decryptor, evaluator, encoder);
      cout<<"------------------"<<i<<"----------------------------------"<<endl;
  }
}                                                                                                                                                                
//5
/*
void erreur(vector<vector<Ciphertext>> &model_enc,                                                                                                               
            float &loss){                                                                                                                                        
}                                                                                                                                                                
//8                                                                                                                                                              
void predict(vector<vector<Ciphertext>> &model_enc,                                                                                                              
Ciphertext &result_enc){                   
}*/                                                                                                                                                                       
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
  vector<float> model{1,0.5,1,1,1,1,1,1,1};
  vector<Ciphertext> model_enc;
  vector< vector<Ciphertext> > data_enc;
  string csv_file_name = "pima.csv";
  read_csv(csv_file_name,csv_data);
  encrypt_data(csv_data, data_enc, encryptor, decryptor, evaluator, encoder);
  model_encryption(model, model_enc,encryptor, decryptor, evaluator, encoder);
  generator.generate_evaluation_keys(1);
  train (data_enc, model_enc, encryptor, decryptor, evaluator, encoder);
  for(int j=0; j<model_enc.size();++j)
    cout<<"theta ["<<j<<"] = "<<encoder.decode(decryptor.decrypt(model_enc[j]))<<endl;
  return 0;
}
