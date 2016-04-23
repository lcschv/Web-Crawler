#include <string>
#include <iostream>
using namespace std;
#include <algorithm>    // std::make_heap, std::pop_heap, std::push_heap, std::sort_heap
#include <vector>       // std::vector
#include <queue>
#include <sstream>
#include <istream>
#include <unordered_map>
#include <thread> 
#include <mutex>
#include <iterator>
#include <functional>
#include "include/CkString.h"
#include "include/CkStringArray.h"
#include "include/CkSpider.h"

struct Links{
    string url;
    int level;
};

struct PoliteLink {
	Links link;
	long blockedUntil;
};

/*Comparador para a lista de prioridade, utiliza o valor do Level como comparador.*/
struct cmp {
    bool operator()(const Links a, const Links b){return a.level > b.level;}
};

struct cmpPolite {
    bool operator()(const PoliteLink a, const PoliteLink b){return a.blockedUntil > b.blockedUntil;}
};

class MyQueue {

	private:

		int pops;
		CkSpider MyQueueSpider;
		CkString tempUrl,aux1,aux2,aux3,aux4,newDomain;
		CkString tempBaseDomain;
		double currentTime, lastTimeAccess, newTimeAccess;		
		vector<Links> PriorityQueue;
		//long tempoEspera, tempoLimite;
		vector<PoliteLink> PolitenessQueue;
		
		mutex mtx;
	public:
		unordered_map <string, bool> AlreadyAddedUrls;
		unordered_map <string, long> DomainHash;
		
	
		double startTime;

		MyQueue() {
			aux1 = "http://";
			aux2 = "https://";
			aux3 = "www.";
			aux4 = "ftp://";
			pops = 0;
			startTime = this->get_wall_time();
			//tempoEspera = 20;
			//tempoLimite = 100;
		};

		Links pop();

		int push(Links link);
		double get_wall_time();
		long get_wall_time_long();
		Links MyHeapPop();
		void MyHeapPush(Links link);
		void MyPoliteHeapPush(PoliteLink link);
		PoliteLink MyPoliteHeapPop();
		void imprimir();
		// int sizeheap();
};
