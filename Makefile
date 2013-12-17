SRCS=MosaitPixel.cpp MosaitImg.cpp MosaitMain.cpp MosaitCentroid.cpp

OBJS=MosaitPixel.o MosaitImg.o MosaitMain.o MosaitCentroid.o

CFLAGS= -ggdb
#CFLAGS= -ggdb -O2 
#CFLAGS= -O2
CXXFLAGS= -ggdb
#CXXFLAGS= -ggdb -O2 
#CXXFLAGS= -O2
LFLAGS= -lfreeimageplus

CC=g++ 

OUTPUT=mosait

$(OUTPUT): $(OBJS) Makefile
	$(CC) -o $(OUTPUT) $(OBJS) $(CFLAGS) $(LFLAGS)

#Makefile: clean

clean:
	rm -f *.o $(OUTPUT) 
