
CXXFLAGS = -I include lib/libchilkat-9.5.0.so -lpthread

crawler: crawler.o myqueue.o
	g++ -std=c++11 myqueue.o crawler.o $(CXXFLAGS) -o crawler


crawler.o: crawler.cpp myqueue.h include/CkSpider.h include/CkString.h include/CkStringArray.h
	g++ -std=c++11 -c crawler.cpp

myqueue.o: myqueue.cpp myqueue.h include/CkSpider.h include/CkString.h include/CkStringArray.h
	g++ -std=c++11 -c myqueue.cpp


