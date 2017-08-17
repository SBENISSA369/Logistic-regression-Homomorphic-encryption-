#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <sstream>

using namespace std;
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
float sigmoid(vector<float>&  X,
	     vector<float>& model){
  cout<<"X.size() = "<<X.size()<<endl;
  X.insert(X.begin(),1);
  cout<<"X.size() = "<<X.size()<<endl;
  
  float somme = 0;
  for(int i = 0; i<X.size()-1; ++i){
    somme += X[i] * model[i];
  }
  cout<<"somme = "<<somme<<endl;
  return(1/(1+exp(-somme)));
}
void SGD(vector<float>& sample,
	 vector<float>& model, float lr){
  float sigmoid_class_lr = (sigmoid(sample,model)-sample[sample.size()-1]) * lr;
  model[0] = model[0] - sigmoid_class_lr;
  for(int i = 1; i < model.size(); ++i){
    float sigmoid_class_mult = sigmoid_class_lr * sample[i];
    model[i] = model[i] - sigmoid_class_mult;
  }
}
void train(vector<vector<float>>& data,
	   vector<float>& model){
  float lr = 0.001;
   for (int number_iter = 0; number_iter<1000; ++number_iter ){
               for(int i = 0; i < data.size(); ++i)
               SGD(data[i], model,lr);
	       	        }
}
int main(){
  vector<vector<float>> csv_data;
  vector<float> model{1,1,1,1,1,1,1,1,1};
  string csv_file_name = "pima.csv";
  read_csv(csv_file_name, csv_data);
  float t = sigmoid(csv_data[0],model);
  cout <<"sig = "<<t<<endl;
  //train(csv_data, model);
  //for(int i = 0; i<model.size();++i)
  // cout<<"model ["<<i<<"] = "<<model[i]<<endl;
    }
