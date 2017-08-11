BIN_DIR=/home/sami/sealcrypto/bin
LIB_DIR=/home/sami/sealcrypto/bin
INCLUDE_DIR=/home/sami/sealcrypto/SEAL
SRCS=LogisticRegression_seal_vect.cpp
RES =/home/sami/Logistic-regression-Homomorphic-encryption-
SEALRUN=$(RES)/LogiscticRegression_seal
CXX=g++
CXXFLAGS=-march=native -O3 -std=c++11
LDFLAGS=

.PHONY : all clean

all : $(SEALRUN)

$(SEALRUN) : $(SRCS)
	@-mkdir -p $(dir $@)
	$(CXX) $(SRCS) $(CXXFLAGS) $(LDFLAGS) $(addprefix -I,$(INCLUDE_DIR)) $(addprefix -L,$(LIB_DIR)) -lseal -o $@

clean :
	@-rm -f $(OBJS) $(SEALRUN)
