

programma : classimain.o classi.o 
	g++ -o programma classimain.o classi.o

# Compiling source files

classi.o : classi.cpp classi.h
	g++ -c classi.cpp

classimain.o : classimain.cpp classi.h
	g++ -c classimain.cpp

clean :
	rm -f programma *.o

