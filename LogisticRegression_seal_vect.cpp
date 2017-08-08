//1
void read_csv(string csv_file_name,
	      vector<vector<float>> &csv_data){
}
//2
void encrypt_data(const vector<vector<float>> &csv_data,
		  vector<vector<Ciphertext>> &data_enc){
}

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
}
//4
Ciphertext sigmoid_enc(vector<Ciphertext> &X_enc){
}
//3
vector<Ciphertext> der_sigmoid_enc(vector<Ciphertext> &X_enc){
}


  
