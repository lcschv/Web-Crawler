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


/*Realiza a contagem de níveis de uma dada URL. Essa contagem é feita da seguinte maneira, 
primeiramente efetuo a remoção de protolos e www's. Após feito isso, conto a quantidade de 'Pontos' 
contidas no URLDomain e BaseDomain, realizo a subtração entre BaseDomain-UrlDomain, dessa forma eu verifico a quantidade de
níveis existentes no domínio. Em seguida, verifico a quantidade de '/' e multiplico por 10. 
Realizo a soma desses valores e obtenho assim o level do url passado. Por último, retorno o Link; 
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
	return newLink;	
}


/* Adiciono à Priority Queue todos os items das listas Unspired e Outboundlinks. Essa adição é realizada pelo método
push() da classe MyQueue. Passo assim, a chamada queueController->push(getLevelUrl(nextUnspidered)) para
cada um dos items de ambas as litas.
É verificado a existência de ".br e "br.", o que me garante a coleta apenas de páginas do domínio Brasileiro. */    

void MoveLinksToPriorityQueue(CkSpider *spider, MyQueue *queueController){
	
	int i;
	CkString url;
	string teste;
	CkString nextUnspidered;
	CkString domain;

	int total_unspidered = spider->get_NumUnspidered();
    for (i = 0; i < total_unspidered; i++)
    {    
		spider->GetUnspideredUrl(0, nextUnspidered);
	   	queueController->push(getLevelUrl(nextUnspidered));
        spider->SkipUnspidered(0);
	 }

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
    }
    spider->ClearOutboundLinks();
}


/*Realiza a escrita do buffer para o arquivo.
Em seguida realiza o shrink_to_fit() para economia de espaço de memória.
*/
void WriteBufferToFile(vector<string> *buffer, string namefile){

	ofstream filename;
	filename.open(namefile.c_str());
	while (!buffer->empty()){
			// ||| URL1 | HTML1 ||| 
			filename << buffer->back() <<"\n";	
			buffer->pop_back();
	}
	buffer->shrink_to_fit();
	filename.close();
}

/* Função principal do programa. Cada uma das threads chama essa função. Cada uma das 
threads possui seu próprio CkSpider.
Declara os buffers, nome dos arquivos, alguns auxilares..
Cria um loop infinito, verifica se a PriorityQueue está vazia, se estiver faz com que a thread durma por 2 segundos..
Remove os http, https, etc.. para o cálculo de Dominios.
*/
void Crawling(int ThreadNum, MyQueue *queueController){

	vector<string> *buffer = new vector<string>;
	CkSpider *spider = new CkSpider();
	int countfile;
	int sec;

	Links nextUrlToBeSpired;  
	CkString url, lastUrl,lastTitle, lastHtml,nextUnspidered, newdomain,tempUrl;
	string lastUrlString, lastHtmlString, tempString, newdomainstr;
	double currentTime;
	int i, count = 0;
	string namefile = "files/data_" + to_string(ThreadNum) + "_thread-out";
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

		// removing https, http, ftp.. etc
		tempUrl = nextUrlToBeSpired.url.c_str();
		tempUrl.removeFirst(aux1);
		tempUrl.removeFirst(aux2);
		tempUrl.removeFirst(aux3);
		tempUrl.removeFirst(aux4);
		spider->GetUrlDomain(tempUrl,newdomain);
		newdomainstr = spider->getBaseDomain(newdomain);
		
		/* Realiza o initialize do Domínio recebido ..se não realizar o initialize todas as vezes dá error, algumas
		 paginas não são coletadas..*/
		spider->Initialize(newdomainstr.c_str());
		

		spider->AddUnspidered(nextUrlToBeSpired.url.c_str());
		/*Adicionei alguns Patterns a ser evitados nos links, pois há casos que caiam em "share, perfil de twitter, etc"
		Apenas urls importantes serão coletadas.
		*/
		spider->AddAvoidPattern("*twitter*");
	    spider->AddAvoidPattern("*facebook*");
	    spider->AddAvoidPattern("*registro*");
	    spider->AddAvoidPattern("*calendar*");
	    spider->AddAvoidPattern("*jora*");
	    spider->AddAvoidPattern("*blogspot*");
		spider->AddAvoidPattern("*=*");	
		spider->AddAvoidPattern("*amp*");

		currentTime= queueController->get_wall_time();

		
		// Realiza o CrawlNext() para procurar por novos links..
		spider->CrawlNext();
		
		//Função para mover os links encontrados pelo CrawlNext() para a PriorityQueue.
		MoveLinksToPriorityQueue(spider, queueController);

		// Pega o URL, HTML e Titulo da página ..
		spider->get_LastUrl(lastUrl);
	    spider->get_LastHtml(lastHtml);
	    lastUrlString = lastUrl.getString();
	    lastHtmlString = lastHtml.getString();
	    //Adiciona ao buffer o Url e HTML da pagina coletada
	    tempString = "||| " + lastUrlString + " | " + lastHtmlString;
	    buffer->push_back(tempString);


	    // Verifica se o buffer já possui 100páginas coletadas, se sim escreve em disco.
	    if (buffer->capacity()> 100){
	    	string namefile2 = namefile + "_" + to_string(countfile);
	    	WriteBufferToFile(buffer, namefile2);
	    	countfile++;

	    //Reinstancia spider, porque antes estava tendo problemas .. ficava lento após certo tempo.
		delete (spider);
		spider = new CkSpider();
	    }
	}
}

/*Inicializa a PriorityQueue adicionando as seeds a Queue.*/
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
	MyQueue queueController;
	InitializeCrawler(&queueController);


	// Declarado as Threads, cada thread chama a função Crawling..
    for (int ThreadNum = 0; ThreadNum < num_threads; ++ThreadNum) {
    	t[ThreadNum] = thread(Crawling, ThreadNum, &queueController);
    }
    
    for (int ThreadNum = 0; ThreadNum < num_threads; ++ThreadNum) {
	    t[ThreadNum].join();
    }

}
