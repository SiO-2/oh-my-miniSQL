# .PHONY : all clean
# all: Makefile 
# 	$(MAKE) -C main
# 	g++ -o MiniSQL *.o main/*.o
# wyc: …… I give up, I can not run makefile successfully in my computer
main.o: main.cpp Interpreter.cpp Basicop.cpp SqlError.cpp MiniSQL.cpp CatalogManager.cpp
	g++ -o main main.cpp Interpreter.cpp Basicop.cpp SqlError.cpp MiniSQL.cpp CatalogManager.cpp

Interpreter.o: SqlError.cpp Basicop.cpp Interpreter.cpp
	g++ -c Interpreter.cpp Basicop.cpp SqlError.cpp

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

# clean : 
# 	rm *.o
.PHONY : clean
clean :
	-rm *.o