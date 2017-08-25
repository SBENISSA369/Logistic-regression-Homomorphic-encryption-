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
#include <algorithm> 
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
float max_value(vector <float>& temp ){
  int k=0;
  float max = 0;
  while(k<temp.size()){
    if(temp[k]>max)
            max = temp[k];
       k=k+1;
  }
    return max;
  }
float sigmoid(const vector<float>&  X,
	      vector<float>& model){
  float somme = 0;
  for(int i = 0; i<X.size()-1; ++i){
    somme += X[i] * model[i+1];
  }
  somme += model[0];
  return(1/(1+exp(-somme)));
}

void scale(vector < vector<float>> & csv_data,
          vector < vector<float>> & data_norm){
        vector<float> maxi;
        vector<float> test;
        vector<float> temp;
        float max = 0;
        for(int j=0; j<csv_data[j].size(); ++j){
                    for(int i=0; i<csv_data.size(); ++i){
                          temp.push_back(csv_data[i][j]);
                       }
               float max_v=max_value(temp);
               maxi.push_back(max_v);       
               temp.clear();
        }
        for(int i=0; i<csv_data.size(); ++i){
               for(int j=0; j<csv_data[i].size(); ++j)
               test.push_back(csv_data[i][j]/maxi[j]);
               data_norm.push_back(test);
               test.clear();
}
}
void split(vector < vector<float>> & data_norm,
	   vector < vector<float>> & training,
	   vector < vector<float>> & testing){
  vector<int> indice;
  srand ( unsigned ( time(0) ) );
  for (int i=0; i<data_norm.size(); ++i) indice.push_back(i);
  random_shuffle(indice.begin(), indice.end());
  int t= indice.size()*0.2;
  for(int j =0; j<indice.size();++j){
    if(j < t)  testing.push_back(data_norm[j]);
    else     training.push_back(data_norm[j]);
  }
 }
void predict(vector < vector<float>> & testing,
	     vector <float> & model,
	     vector <bool> & pred){
  for(int k=0; k<testing.size(); ++k){
    if( sigmoid(testing[k],model) > 0.5 ) pred.push_back(1);
    else  pred.push_back(0);
     }
 

	   }
void accuracy(vector < vector<float>> & testing,
	      vector <bool> & pred,
	      float & acc){
  int total = 0;
  for(int i=0; i < testing.size(); ++i){
              if(testing[i][testing[i].size()-1] == pred[i]){
                   total = total + 1;
               }
   }
    acc = (float(total)/pred.size()) * 100;
}
void SGD(const vector<float>& sample,
	 vector<float>& model, float lr){
  float T = sigmoid(sample,model);
  float sigmoid_class_lr = (T-sample[sample.size()-1]) * lr;
  model[0] = model[0] - sigmoid_class_lr;
  for(int i = 1; i < model.size(); ++i){
  float sigmoid_class_mult = sigmoid_class_lr * sample[i-1];
    model[i] = model[i] - sigmoid_class_mult;
  }
  }
float erreur(const vector<vector<float>>& data,vector<float>& model){
  float Erreur = 0;
       for(int i=0; i<data.size(); ++i){
    float sig = sigmoid(data[i], model);
    Erreur += data[i][data[i].size()-1]*log(sig)+(1-data[i][data[i].size()-1])*log(1-sig);
        }
   return(-(Erreur/data.size()));
  }

void train(const vector<vector<float>>& data,
	   vector<float>& model){
  float lr = 0.0001;
  float loss = 0;
  string const nomFichier("loss.csv");
  ofstream monFlux(nomFichier.c_str());
  
for (int number_iter = 0; number_iter<50000; ++number_iter ){
  for(int i = 0; i < data.size(); ++i)
                    SGD(data[i], model,lr);
  loss = erreur(data, model);
   monFlux << number_iter << " , " << loss<< endl;
   }

}
int main(){
  vector<vector<float>> csv_data;
  vector<vector<float>> data_norm;
  vector<vector<float>> training;
  vector<vector<float>> testing;
  vector<bool> pred;
  float acc;
  vector<float> model{0.1,0.2,-0.1,0.3,0.5,0.6,0.7,-0.2,0.3};
  string csv_file_name = "pima.csv";
  read_csv(csv_file_name, csv_data);
  scale(csv_data,data_norm);
  split(data_norm, training, testing);
  train(training, model);
  predict(testing, model, pred);
  for(int i=0; i<model.size(); ++i) cout<<"model ["<<i<<"] = "<<model[i]<<endl;
  accuracy(testing, pred, acc);
  cout <<"accuracy = "<< acc <<endl;
 }
