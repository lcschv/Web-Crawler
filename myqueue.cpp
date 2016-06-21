#include "myqueue.h"
#include "iostream"
#include <time.h>
#include <sys/time.h>



// Metodo que imprime todos os items contidos na Politeness Queue.
void MyQueue::imprimir(){ 


	while (!PolitenessQueue.empty()){
 		PoliteLink tempVari;
 		tempVari = MyPoliteHeapPop();
 		cout<<"blocked: "<< tempVari.blockedUntil<<"url: "<< tempVari.link.url<<" level: "<< tempVari.link.level <<endl;
	}

	cout << "vazio\n";

}


/* Método para realizacão da extração do próximo link a ser coletado.
Esse método realiza a extração de ambas as Queues, Politeness e Priority.
Primeiramente tenta extrair da Politeness, ou seja, verifica se algum dos links adicionados
anteriormente já esperaram tempo suficiente para atender a condição de Politeness. Caso nenhum dos links
da Politeness já tenham aguardado suficiente, é extraído da PriorityQueue.
Vale ressaltar, que ao extrair da PriorityQueue é verificado se o tempo atual 
é maior que o BlockedUntil(últimoacesso+tempoEspera), se for maior retorna o nextUrlToBeSpired, caso contrário,
realiza o push do Link na PolitenessQueue e chama-se o método Pop() novamente.
*/
Links MyQueue::pop(){
	Links nextUrlToBeSpired;

	this->mtx.lock();
	
	/*Tempo de Politeness*/
	long tempoEspera = 30;
	long tempoLimite = 150;
	

	// Verifica se o Link já pode ser acessado .. 
	if ((!this->PolitenessQueue.empty()) && (this->PolitenessQueue.front().blockedUntil <= this->get_wall_time_long())){
		nextUrlToBeSpired = MyPoliteHeapPop().link;
	
	} else {

		// Estratégia para fazer as Threads dormirem enquanto a PriorityQueue estiver vazia ..
		if (PriorityQueue.empty()){
			nextUrlToBeSpired.level = -1;
			nextUrlToBeSpired.url = "";
			this->mtx.unlock();
			return nextUrlToBeSpired;
		}

		// Próximo item a ser coletado..
		nextUrlToBeSpired = MyHeapPop();

		/*Estou removendo http, https, ftp e www. pois estava obtendo alguns problemas ao utilizar a biblioteca
		para obter o Domínio da URL. Removendo esses itens antes de chamar o método getBaseDomain() solucionou 
		o problema.	
		*/
		tempUrl = nextUrlToBeSpired.url.c_str();
		tempUrl.removeFirst(aux1);
		tempUrl.removeFirst(aux2);
		tempUrl.removeFirst(aux3);
		tempUrl.removeFirst(aux4);
		MyQueueSpider.GetUrlDomain(tempUrl,newDomain);
		tempBaseDomain = MyQueueSpider.getBaseDomain(newDomain);
		std::string tempBaseDomainStr = tempBaseDomain.getString();

		// Tempo atual..
		this->currentTime = get_wall_time();
		
		/*Se não existe o domínio na DomainHash, ou seja, primeiro acesso, adiciona-se
		ao seu valor o tempo atual + tempoEspera, dessa maneira já fica salvo na Hash quando
		o próximo link pode acessar.
		*/
		if (!DomainHash[tempBaseDomainStr]) {
			DomainHash[tempBaseDomainStr] = get_wall_time_long()+ tempoEspera;
		
		/* Quando a condição de politeness já foi respeitada, apenas atualiza o tempo na DomainHash.*/
		} else if (DomainHash[tempBaseDomainStr] <= get_wall_time_long()) {
			DomainHash[tempBaseDomainStr] = get_wall_time_long() + tempoEspera;

		
		/*Quando é retornado um URL de um domínio acessado em um intervalo de tempo menor que 30s, adiciona-se + 30s
		a DomainHash[dominio], da o push do link na PolitenessQueue com o BlockedUntil = DomainHash[tempBaseDomainStr] + tempoEspera;
		 e chama o método Pop() novamente
		*/		
		} else {

			if ((this->PolitenessQueue.capacity()<10000)&&(DomainHash[tempBaseDomainStr]<get_wall_time_long()+ tempoLimite)){

				DomainHash[tempBaseDomainStr] = DomainHash[tempBaseDomainStr] + tempoEspera;	
				PoliteLink politel;
				politel.link = nextUrlToBeSpired;
				politel.blockedUntil = DomainHash[tempBaseDomainStr];

				MyPoliteHeapPush(politel);
			}

			this->mtx.unlock();
			return this->pop();
		}
		
	}
	this->pops++;
	this->mtx.unlock();
	std::cout <<"Time: " << currentTime - this->startTime << "  ---- Url: " << nextUrlToBeSpired.url << "  ----- NumPages: "<<this->pops << std::endl;
	return nextUrlToBeSpired;
};


/* Método para controlar o Push dos items na PriorityQueue, controle de condição de corrida
Seta 1 na Hash AlreadyAddedUrls, para evitar repetições de links.
*/
int MyQueue::push(Links link){
	this->mtx.lock();
	if (!this->AlreadyAddedUrls[link.url] && this->PriorityQueue.capacity()<10000){
		this->AlreadyAddedUrls[link.url] = 1;
		this->MyHeapPush(link);
		this->mtx.unlock();
		return true;
	}
	this->mtx.unlock();
	return false;
};

/* Método push normal da PriorityQueue, apenas re-implementado para utilização de vector, 
onde cmp é a função de comparação, onde é levado em consideração o nível do URL.
*/

void MyQueue::MyHeapPush(Links link) {
    this->PriorityQueue.push_back(link);
    push_heap(PriorityQueue.begin(), PriorityQueue.end(), cmp());
};


/* Método push normal para PolitenessQueue, apenas re-implementado para utilização de vector
cmpPolite é a função de comparação, onde é comparado o Tempo de BlockedUntil, dessa forma, com menores
valores de BlockedUntil devem sair primeiro.
*/
void MyQueue::MyPoliteHeapPush(PoliteLink link) {
    this->PolitenessQueue.push_back(link);
    push_heap(PolitenessQueue.begin(), PolitenessQueue.end(), cmpPolite());
};




Links MyQueue::MyHeapPop() {

    Links tempLink = this->PriorityQueue.front();
    //Essa operação irá mover o link de menor nível para o fim do vector.
    pop_heap(this->PriorityQueue.begin(), this->PriorityQueue.end(), cmp());
 

    //Remove o último elemento do vector, o qual é a URL de menor nível;
    this->PriorityQueue.pop_back();  
	PriorityQueue.shrink_to_fit();
    return tempLink;
};


PoliteLink MyQueue::MyPoliteHeapPop() {

    PoliteLink tempLink = this->PolitenessQueue.front();
     
    //Essa operação irá mover o link de menor nível para o fim do vector.
    pop_heap(this->PolitenessQueue.begin(), this->PolitenessQueue.end(), cmpPolite());
 
    //Remove o último elemento do vector, o qual é a URL de menor BlockedUntil(tempo);
    this->PolitenessQueue.pop_back();  
	PolitenessQueue.shrink_to_fit();
    return tempLink;
};

/*Funcão de cálculo de tempo em microsegundos*/
double MyQueue::get_wall_time() {
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * 0.000001;
};


/*Função para cálculo de tempo em sec*/
long MyQueue::get_wall_time_long() {
    struct timeval time;
    if (gettimeofday(&time,NULL)) {
        //  Handle error
        return 0;
    }
    return (long) time.tv_sec;
};


