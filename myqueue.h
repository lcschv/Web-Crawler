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

/* Essa é a struct Links, level é o nível da URL*/
struct Links{
    string url;
    int level;
};

/*PoliteLink, possui um link e BlockedUntil que é o Tempo em que aquela URL poderá ser coletada*/
struct PoliteLink {
	Links link;
	long blockedUntil;
};

/*Comparador para a lista de prioridade, utiliza o valor do Level como comparador.*/
struct cmp {
    bool operator()(const Links a, const Links b){return a.level > b.level;}
};


/*Comparador para a PolitenessQueue, utiliza o valor do BlockedUntil, ou seja, menor tempo sai primeiro*/
struct cmpPolite {
    bool operator()(const PoliteLink a, const PoliteLink b){return a.blockedUntil > b.blockedUntil;}
};



/*Essa classe tem como principal objetivo controlar a Priority Queue, 
evitando condições de corrida entre as Threads e as condições de Politeness
Nela são declarados as duas Hashs PriorityQueue e PolitinessQueue.
São declarados as Hashs de controle de links repitidos AlreadyAddedUrls e DomainHash para
controle de acessos a URLs de mesmo domínio. Mais detalhes sobre os métodos encontra-se no .cpp.
*/
class MyQueue {

	private:

		int pops;
		CkSpider MyQueueSpider;
		CkString tempUrl,aux1,aux2,aux3,aux4,newDomain;
		CkString tempBaseDomain;
		double currentTime, lastTimeAccess, newTimeAccess;		
		vector<Links> PriorityQueue;
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
