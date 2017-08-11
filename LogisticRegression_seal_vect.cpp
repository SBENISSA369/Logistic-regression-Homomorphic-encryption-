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
      while( getline( buffer, token, ',' ) )
	{
	  temp.push_back(strtof((token).c_str(),0));
	}
      csv_data.push_back(temp);
      temp.clear();
    }
  }

}

//2
void encrypt_data(vector<vector<float>> &csv_data,
		  vector<vector<Ciphertext>> &data_enc){
  EncryptionParameters parms;
  parms.set_poly_modulus("1x^2048 + 1");
  parms.set_coeff_modulus(ChooserEvaluator::default_parameter_options().at(2048));
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
  vector<Ciphertext> encrypted_temp;
  for (int x = 0; x < csv_data.size(); ++x)
    {
            for (int y = 0; y < csv_data[x].size(); ++y)
	{
	  Plaintext encoded_number = encoder.encode(csv_data[x][y]);
	  encrypted_temp.emplace_back(encryptor.encrypt(encoded_number));
	
	}
     data_enc.push_back(encrypted_temp);
     encrypted_temp.clear();
    }
    for(int i=0 ; i< data_enc[1].size() ; i++){
    Plaintext test1 = decryptor.decrypt(data_enc[1][i]);
    float test_1= encoder.decode(test1);
    cout <<"X_enc : "<<i<<" = "<<test_1<<endl;
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
Ciphertext sigmoid_enc(vector<Ciphertext>& X_enc, vector<Ciphertext>& model_enc){
  EncryptionParameters parms;
  parms.set_poly_modulus("1x^2048 + 1");
  parms.set_coeff_modulus(ChooserEvaluator::default_parameter_options().at(2048));
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
  X_enc.insert( X_enc.begin(), encryptor.encrypt(encoder.encode(1)));
  vector<Ciphertext> vect_somme;
  for(int i=0 ; i< X_enc.size()-1 ; i++){
    Ciphertext mult = evaluator.multiply(X_enc[i], model_enc[i]);
    Plaintext test1 = decryptor.decrypt(X_enc[i]);
    float test_1= encoder.decode(test1);
    cout <<"X_enc : "<<i<<" = "<<test_1<<endl;
    Plaintext test2 = decryptor.decrypt(model_enc[i]);
    float test_2= encoder.decode(test2);
    cout <<"model_enc : "<<i<<" = "<<test_2<<endl;
    
    Plaintext test6 = decryptor.decrypt(mult);
    float test_6= encoder.decode(test6);
    cout <<"mult : "<<i<<" = "<<test_6<<endl;
    
    vect_somme.push_back(mult);
    }
  Ciphertext somme = evaluator.add_many(vect_somme);
  return somme;
  
    }
void model_encryption(vector<float>& model,
                      vector<Ciphertext>& model_enc){
  EncryptionParameters parms;
  parms.set_poly_modulus("1x^2048 + 1");
  parms.set_coeff_modulus(ChooserEvaluator::default_parameter_options().at(2048));
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
  cout<<"----------------"<<endl;
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
  parms.set_poly_modulus("1x^2048 + 1");
  parms.set_coeff_modulus(ChooserEvaluator::default_parameter_options().at(2048));
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
  Ciphertext result = encryptor.encrypt(encoder.encode(0));
  string csv_file_name = "pima.csv";
  read_csv(csv_file_name,csv_data);
  encrypt_data(csv_data, data_enc);
  model_encryption(model, model_enc);
   for(int i=0 ; i< data_enc[1].size() ; i++){
   Plaintext test1 = decryptor.decrypt(data_enc[1][i]);
    float test_1= encoder.decode(test1);
    cout <<"X_enc : "<<i<<" = "<<test_1<<endl;
    }
  //result = sigmoid_enc(data_enc[1], model_enc);
  
     return 0;
}


  
