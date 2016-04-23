#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
using namespace std;
#include <algorithm>    // std::make_heap, std::pop_heap, std::push_heap, std::sort_heap
#include <vector>       // std::vector
#include <queue>
#include <sstream>
#include <istream>
#include <unordered_map>
#include <thread> 
#include <time.h>
#include <unistd.h>
#include <mutex>
#include "myqueue.h"




/*Declaração da estrutura de comparação dos links para verificação de level do URL. */

#define num_threads 100


/*É criada a PriorityQueue de Structs Links. */


/*Realiza a contagem de níveis de uma dada URL. Essa contagem é feita da seguinte maneira, primeiramente efetuo a remoção 
de protolos e www's. Após feito isso, conto a quantidade de 'Pontos' até o primeiro /, dessa forma eu verifico a quantidade de
níveis existentes no domínio. Em seguida, verifico a quantidade de '/' que o link possui, realizo a soma desses valores e obtenho assim
o level do url passado. Por último, faço a inserção desse url na PriorityQueue. 
*/
Links getLevelUrl(CkString url){
	CkSpider tempSpider;
	int i = 0,level = 0;
	CkString urls, DomaintempUrl,BaseDomainTemp,tempUrl, checkbr;
	tempUrl = url;
	CkString aux1, aux2, aux3, aux4;
	aux1 = "http://";
	aux2 = "https://";
	aux3 = "www.";
	aux4 = "ftp://";
	tempUrl.removeFirst(aux1);
	tempUrl.removeFirst(aux2);
	tempUrl.removeFirst(aux3);
	tempUrl.removeFirst(aux4);

	tempSpider.GetUrlDomain(tempUrl,DomaintempUrl);
	BaseDomainTemp = tempSpider.getBaseDomain(url);
//	cout << "BaseDomain: " << BaseDomainTemp.getString() <<" URLDOMAIN: "<< DomaintempUrl.getString()<<endl;

	level = DomaintempUrl.countCharOccurances('.') - BaseDomainTemp.countCharOccurances('.');
	if (tempUrl.getNumChars()>0){
	tempUrl.shorten(1);
	}
	level += 10 * tempUrl.countCharOccurances('/');

	Links newLink;

	newLink.url = url.getString();
	newLink.level = level;
//	cout<< "sai do getlevel"<<endl;
	return newLink;	
}


/* Adiciono à Priority Queue todos os items da Unspired e outboundlinks. Essa adição é realizada pela
função getLevelUrl(), nela já é realizado o cálculo do nível do link e a adição
a PriorityQueue de acordo com a comparação do nível do link a ser adicionado.*/    

void MoveLinksToPriorityQueue(CkSpider *spider, MyQueue *queueController){
	
	int i;
	CkString url;
	string teste;
	CkString nextUnspidered;
	CkString domain;

	int total_unspidered = spider->get_NumUnspidered();
    for (i = 0; i < total_unspidered; i++)
    {    
  //       string aux5 = ".br/";
		// string aux6 = "/br.";
		spider->GetUnspideredUrl(0, nextUnspidered);
		// teste = nextUnspidered.getString();
		// size_t checkbr1 = teste.find(aux5);
		// size_t checkbr2 = teste.find(aux6);
		// if (checkbr1 != string::npos || checkbr2 != string::npos){
	    queueController->push(getLevelUrl(nextUnspidered));
        //}
        spider->SkipUnspidered(0);
   	//cout <<"dei pau no moves, unpired";
	 }
//	cout << "sai dos unspired .."<<endl;
    for (i = 0; i < spider->get_NumOutboundLinks(); i++)
    {
        spider->GetOutboundLink(i, url);
	    string aux5 = ".br/";
		string aux6 = "/br.";
		teste = url;
		size_t checkbr1 = teste.find(aux5);
		size_t checkbr2 = teste.find(aux6);
		if (checkbr1 != string::npos || checkbr2 != string::npos){
        	queueController->push(getLevelUrl(url));
    	}
	//cout << "dei pau nos outbound";
    }
//	cout << "sai dos outbound .."<<endl;
    spider->ClearOutboundLinks();
}
void WriteBufferToFile(vector<string> *buffer, string namefile){

	ofstream filename;
	filename.open(namefile.c_str());
	while (!buffer->empty()){
			// ||| URL1 | HTML1 ||| 
			filename << buffer->back() <<"\n";	
			buffer->pop_back();
	}
	buffer->shrink_to_fit();
	//delete buffer;
	//buffer = new vector<string>;
	filename.close();
//	cout << "escrevi no arquivo .."<<endl;
}

void Crawling(int ThreadNum, MyQueue *queueController){

	vector<string> *buffer = new vector<string>;
	CkSpider *spider = new CkSpider();
	int countfile;
	//mutex mtx;
	int sec;

	// string namefile = to_string(ThreadNum)+"out_thread";

	Links nextUrlToBeSpired;  
	CkString url, lastUrl,lastTitle, lastHtml,nextUnspidered, newdomain,tempUrl;
	string lastUrlString, lastHtmlString, tempString, newdomainstr, olddomainstr = "";
	double currentTime;
	double timediff;
	int i, count = 0;
	string namefile = "files/data_" + to_string(ThreadNum) + "_thread-out";
	//Adiciona para a PriorityQueue todos os Unspired e outboundlinks.
	CkString aux1, aux2, aux3, aux4;
	aux1 = "http://";
	aux2 = "https://";
	aux3 = "www.";
	aux4 = "ftp://";


	while (1){

		nextUrlToBeSpired = queueController->pop();
		while (nextUrlToBeSpired.level == -1) {
			//cout << ThreadNum << " is sleeping...\n";
			usleep( 2000000 );
			nextUrlToBeSpired = queueController->pop();			
		}
		tempUrl = nextUrlToBeSpired.url.c_str();
		tempUrl.removeFirst(aux1);
		tempUrl.removeFirst(aux2);
		tempUrl.removeFirst(aux3);
		tempUrl.removeFirst(aux4);
		spider->GetUrlDomain(tempUrl,newdomain);
		newdomainstr = spider->getBaseDomain(newdomain);
		// cout << "GetUrlDomain URL:" << newdomain.getString()<<endl;
		// cout << "basedomain:" << newdomainstr <<endl;
		// cout << "olddomain:" << olddomainstr<< endl;
		// cout << " URL to be unspired:" << nextUrlToBeSpired.url << endl;
		if (newdomainstr.compare(olddomainstr) != 0){
			//cout << "Entrei"<<endl;
			spider->Initialize(newdomainstr.c_str());	
		}
		olddomainstr = newdomainstr;				
		// spider->Initialize(domain.getString());
//		cout <<"URL A SER UNSPIRED: " <<nextUrlToBeSpired.url << endl;	

		spider->AddUnspidered(nextUrlToBeSpired.url.c_str());
		//cout << " URL a ser coletada-> "<< nextUrlToBeSpired.url <<" level: " <<nextUrlToBeSpired.level <<"\n"<<newdomainstr<<"\n";
		spider->AddAvoidPattern("*twitter*");
	    spider->AddAvoidPattern("*facebook*");
	    spider->AddAvoidPattern("*registro*");
	    spider->AddAvoidPattern("*calendar*");
	   // spider.AddAvoidPattern("*page=*");
	    //spider.AddAvoidPattern("*p=*");
	    spider->AddAvoidPattern("*jora*");
	    spider->AddAvoidPattern("*blogspot*");
		spider->AddAvoidPattern("*=*");	
		spider->AddAvoidPattern("*amp*");	
		currentTime= queueController->get_wall_time();
		timediff= currentTime-queueController->DomainHash[newdomainstr];
		//cout <<"DomainHashtime>"<< queueController->DomainHash[newdomainstr];
		//cout <<"TIMEDIFF:" <<timediff<<endl;
		

		spider->CrawlNext();
		std::cout <<"Time: " <<queueController->get_wall_time() - queueController->startTime << " URL: " <<nextUrlToBeSpired.url<< "\n";

//	    cout << currentTime;
		

		MoveLinksToPriorityQueue(spider, queueController);

		spider->get_LastUrl(lastUrl);
	    spider->get_LastHtmlTitle(lastTitle);
	    spider->get_LastHtml(lastHtml);
	    spider->get_LastMethodSuccess();
	    lastUrlString = lastUrl.getString();
	    lastHtmlString = lastHtml.getString();
	    tempString = "||| " + lastUrlString + " | " + lastHtmlString;
	    buffer->push_back(tempString);

	    if (buffer->capacity()> 15){
	    	string namefile2 = namefile + "_" + to_string(countfile);
	    	WriteBufferToFile(buffer, namefile2);
	    	countfile++;
		delete (spider);
//		cout << "oi";
		spider = new CkSpider();
	    }
	    
	   // cout << buffer->capacity()<<endl;
	     // printf("URL[%d]: %s\nTitle: %s\n\n", count, lastUrl.getString(), lastTitle.getString());
	     // cout << ThreadNum;
	      //count++; 
	}
}

void InitializeCrawler(MyQueue *queueController){
	CkSpider spider;
	CkString seed1,seed2,seed3,seed4, seed5, seed6, seed7;
	seed1="http://vocesa.uol.com.br";
	seed3= "http://www.globo.com/";
	seed2= "http://www.catho.com.br";
	seed4 = "http://www.zap.com.br/";
	seed5 = "http://www.infojobs.com.br/";
	seed6 = "http://www.uol.com.br";
	seed7 = "http://estadao.com.br";
	queueController->push(getLevelUrl(seed1));
	
	queueController->push(getLevelUrl(seed2));
	queueController->push(getLevelUrl(seed3));
	queueController->push(getLevelUrl(seed4));
	queueController->push(getLevelUrl(seed5));	
	queueController->push(getLevelUrl(seed6));
	queueController->push(getLevelUrl(seed7));
}	



int main(int argc, char** argv)
{

	CkString urlTeste;
	thread t[num_threads];
	vector<CkString> seeds;
	//getLevelUrl("http://www.catho.com.br");
	MyQueue queueController;
	InitializeCrawler(&queueController);
	// double StartTime = get_wall_time();

    for (int ThreadNum = 0; ThreadNum < num_threads; ++ThreadNum) {
    	t[ThreadNum] = thread(Crawling, ThreadNum, &queueController);
    }
    
    for (int ThreadNum = 0; ThreadNum < num_threads; ++ThreadNum) {
	    t[ThreadNum].join();
    }



//    queueController.imprimir();





   // printf("==================== ROUND 1 - HEAP ====================\n");
    // cout << endl;
    // cout << endl;
    // cout << endl;

//     while (!queueController.PriorityQueue.empty()){
// 		Links tempVari;
// 		tempVari = queueController.pop();
// 		cout<<"url: "<< tempVari.url<<" level: "<< tempVari.level <<endl;
// }


	// urlTeste = "www.facebook.com/arroz/feijao";
	// dominio = spider.getUrlDomain(urlTeste);
	// cout << dominio << endl;	
}
