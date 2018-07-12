LIBS := $(shell pkg-config --libs opencv)

CXX := mpic++
CXX_FLAGS := -Wall -Wextra -pedantic -std=c++03 -O2 -flto 
RUN := mpirun
N_PROC := -np 3
HOSTNAME := -host master,slave01,slave02
NAME := test

all:
	$(CXX) $(CXX_FLAGS) $(NAME).cpp $(LIBS) -o $(NAME)

irun:
	$(RUN) $(N_PROC) $(HOSTNAME) ./$(NAME) lena.jpg

