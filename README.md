# Crawler for Brazilian webpages.
This is an assigment for the course of Information Retrieval - UFMG. This crawler respects politeness of 30 seconds.


#Requirements:

C++11

Ckspider - https://www.chilkatsoft.com/refdoc/vcCkSpiderRef.html


# Crawler Execution
First: make

Second: ./crawler


Para o processo de compilação do código você deverá seguir os seguintes passos:


1 - Realizar o download da Biblioteca http://www.chilkatsoft.com/chilkatLinux.asp e extrair a biblioteca no diretório desejado.

2 - Colocar os arquivos Crawler.cpp, myqueue.cpp, myqueue.h e Makefile dentro da pasta extraida no passo 1, ou seja,  no mesmo diretório em que se encontra as pastas Include e Lib.

3 - Criar um diretório "files" dentro do diretório extraído no passo 1. Esse diretório files conterá as páginas coletadas.

4 - Para realizar a compilação dos códigos execute o comando: make.

5 - Para executar o programa ./crawler .


Para uma melhor vizualização, deixarei imprimindo no terminal os seguintes campos:

TempoDecorrido		UrlColetada 		NúmerodePaginasColetadas

1.00			www.globo.com			1

1.01			www.catho.com.br		2

...			...			       ...


