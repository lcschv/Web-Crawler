#include "myqueue.h"
#include "iostream"
#include <time.h>
#include <sys/time.h>


void MyQueue::imprimir(){ 


	while (!PolitenessQueue.empty()){
 		PoliteLink tempVari;
 		tempVari = MyPoliteHeapPop();
 		cout<<"blocked: "<< tempVari.blockedUntil<<"url: "<< tempVari.link.url<<" level: "<< tempVari.link.level <<endl;
	}

	cout << "vazio\n";

}



Links MyQueue::pop(){
	Links nextUrlToBeSpired;

	
	this->mtx.lock();
	
	long tempoEspera = 30;
	long tempoLimite = 100;
	//if (this->PolitenessQueue.empty()){
	//	 cout << "time: " << this->get_wall_time_long() << " until: " << this->PolitenessQueue.front().blockedUntil << endl;
	//cout << "empty.."<<endl;
	//cout << this->PolitenessQueue.front().blockedUntil<<endl;	



//	cout << "Ta aqui .."<<endl;
	if ((!this->PolitenessQueue.empty()) && (this->PolitenessQueue.front().blockedUntil <= this->get_wall_time_long())){
		nextUrlToBeSpired = MyPoliteHeapPop().link;
	//	cout << "buscou daki\n";
	//	cout << "Chegoudentro..."<<endl;

	} else {

		if (PriorityQueue.empty()){
			nextUrlToBeSpired.level = -1;
			nextUrlToBeSpired.url = "";
			this->mtx.unlock();
			return nextUrlToBeSpired;
		}
		//this->pops+=1;
		nextUrlToBeSpired = MyHeapPop();

		tempUrl = nextUrlToBeSpired.url.c_str();
		tempUrl.removeFirst(aux1);
		tempUrl.removeFirst(aux2);
		tempUrl.removeFirst(aux3);
		tempUrl.removeFirst(aux4);
		MyQueueSpider.GetUrlDomain(tempUrl,newDomain);
		tempBaseDomain = MyQueueSpider.getBaseDomain(newDomain);

		std::string tempBaseDomainCorreta = tempBaseDomain.getString();

		this->currentTime = get_wall_time();
		//this->newTimeAccess = get_wall_time();	
		
		

		if (!DomainHash[tempBaseDomainCorreta]) {
			//std::cout << "1 - time: " <<  get_wall_time_long() << "  "<< tempBaseDomainCorreta << " -> " << nextUrlToBeSpired.url << endl;
		
			DomainHash[tempBaseDomainCorreta] = get_wall_time_long()+ tempoEspera;
			

			//cout << "m: " << m << " k: " << k << " l: " << l << endl;





		} else if (DomainHash[tempBaseDomainCorreta] <= get_wall_time_long()) {

			//std::cout << "2 - time: " <<  get_wall_time_long() << " suposed: " << DomainHash[tempBaseDomainCorreta] << " - "<<   tempBaseDomainCorreta << " -> " << nextUrlToBeSpired.url << endl;
			DomainHash[tempBaseDomainCorreta] = get_wall_time_long() + tempoEspera;	
		} else {


			//std::cout << "3 - " << tempBaseDomainCorreta << " -> " << nextUrlToBeSpired.url << endl;

			if ((this->PolitenessQueue.capacity()<10000)&&(DomainHash[tempBaseDomainCorreta]<get_wall_time_long()+ tempoLimite)){

				DomainHash[tempBaseDomainCorreta] = DomainHash[tempBaseDomainCorreta] + tempoEspera;	
				PoliteLink politel;
				politel.link = nextUrlToBeSpired;
				politel.blockedUntil = DomainHash[tempBaseDomainCorreta];

				MyPoliteHeapPush(politel);
			}

			this->mtx.unlock();
			return this->pop();
		}
		
	}

	this->mtx.unlock();
	// std::cout <<"Time: " << currentTime - this->startTime << " - "<< "Páginas Coletadas: " << this->pops << std::endl;
	// std::cout <t<"URL:" <<nextUrlToBeSpired.url<<std::endl;
//	cout <<"Tamanho da heap: "<<PriorityQueue.capacity();
	return nextUrlToBeSpired;
};

int MyQueue::push(Links link){
	this->mtx.lock();
	if (!this->AlreadyAddedUrls[link.url] /*&& link.level <= 23 */ && this->PriorityQueue.capacity()<10000){
		//this->mtx.lock();
		//cout <<"NivelLink:"<<link.level<<" "<<"link added"<<link.url<<endl;		
		this->AlreadyAddedUrls[link.url] = 1;
			// this->DomainHash[spider->]
		this->MyHeapPush(link);
		this->mtx.unlock();
		return true;
	}
	this->mtx.unlock();
	return false;
};

void MyQueue::MyHeapPush(Links link) {
    this->PriorityQueue.push_back(link);
    push_heap(PriorityQueue.begin(), PriorityQueue.end(), cmp());
};

void MyQueue::MyPoliteHeapPush(PoliteLink link) {
    this->PolitenessQueue.push_back(link);
    push_heap(PolitenessQueue.begin(), PolitenessQueue.end(), cmpPolite());
};

Links MyQueue::MyHeapPop() {

    Links tempLink = this->PriorityQueue.front();
     
    //This operation will move the smallest element to the end of the vector
    pop_heap(this->PriorityQueue.begin(), this->PriorityQueue.end(), cmp());
 
    //Remove the last element from vector, which is the smallest element
    this->PriorityQueue.pop_back();  
	PriorityQueue.shrink_to_fit();
    return tempLink;
};


PoliteLink MyQueue::MyPoliteHeapPop() {

    PoliteLink tempLink = this->PolitenessQueue.front();
     
    //This operation will move the smallest element to the end of the vector
    pop_heap(this->PolitenessQueue.begin(), this->PolitenessQueue.end(), cmpPolite());
 
    //Remove the last element from vector, which is the smallest element
    this->PolitenessQueue.pop_back();  
	PolitenessQueue.shrink_to_fit();
    return tempLink;
};

double MyQueue::get_wall_time() {
    struct timeval time;
    if (gettimeofday(&time,NULL)){
        //  Handle error
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * 0.000001;
};



long MyQueue::get_wall_time_long() {
    struct timeval time;
    if (gettimeofday(&time,NULL)) {
        //  Handle error
        return 0;
    }
    return (long) time.tv_sec;
};


