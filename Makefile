.PHONY : all clean
all: Makefile 
	$(MAKE) -C Interpreter
	g++ -o MiniSQL *.o Interpreter/*.o
	
clean : 
	rm *.o

BufferManager.o : BufferManager.cpp
	g++ -c BufferManager.cpp

CatalogManager.o : CatalogManager.cpp
	g++ -c CatalogManager.cpp

MiniSQL.o : MiniSQL.cpp
	g++ -c MiniSQL.cpp

RecordManager.o : RecordManager.cpp
	g++ -c RecordManager.cpp

# IndexManager.o : IndexManager.cpp
# 	g++ -c IndexManager.cpp