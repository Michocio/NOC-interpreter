/*
*
*	Program zaliczeniowy nr.2
*	Wstęp do Programowania
*	Potok Imperatywny 2015
*
*	Maszyna NOC
*
*	@author Michał Jaroń
*	<mj348711.students.mimuw.edu.pl>
*	nr. indeksu: 348711
* 	@copyright Uniwerstet Warszawski
*	@date: 20.12.2015
* 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


/* 	Zakres "rozsądnych" znaków ASCII,
*	Wszystkie obsługiwane w programie
*	mają mniejszy kod od 126 */
#define MAX_ASCII 126

/* 	Rozmiar maszyny */
#define N_ROZMIAR 256
/*	Mnożnik przy kodowaniu operacji*/
#define KODUJ 8


/* 	Wartości ASCII potrzebne w procesie parsowania */
#define TABULACJA 9 
#define NOWA_LINIA 10 /* \n */
#define SPACJA 32
#define TABLICA 34 /* " */
#define KOMENTARZ 59 /* ; */
#define BACKSLAH 92 /* \ */
#define A_ZERO 97 /* kod ASCII 'a' */

/* 	Liczba nawiasów obsługiwanych
*	jako etykiety strukturalne */
#define NAWIASY 4

/*	Stałe potrzebne przy wykonywaniu
*	kodu maszynowego - pozwalają w łatwy
*	sposób przekazać funkcji jakiego
*	"komperartora" ma używać w działaniu */
#define ROWNE 1
#define NIEROWNE -1

/*	Struktura danych
*	pozwalająca przechowywać
*	wszystkie niezmiennicze wartości
*	potrzebne w programie.
*	Odwzorowuje niejako wszystkie wzorce
*	do których program się odnosi 
*	Są to wartości zdefiniowane w treści zadania*/	
typedef struct wzor
{
	int *pojedyncze;/* Kody operacji bez backslashem */
	int *podwojne;/* Kody operacji z backslashem */
	int *operand;/* Kody operandów*/
	int *nawiasy;/* Nawiasy traktowane jako strukt. */
}wzor;

/*	Struktura ułatwiająca operowanie na
*	danych związanych z obiektami 
* 	pojawiającymi się w programie.
*	Tzn. dane związane z pamiecią maszyny */ 
typedef struct wartosci
{
	int val;/* Wartość elementu */
	int adres;/* Adres w pamięci */
	int typ;/* Opcjonalne */
}wartosci;


/**@}*/
/** @name Elementy interfejsu stos
  @{
 */

/*	Obiekt na stosie*/
typedef struct element
{
	wartosci dane;/* Cechy obiektu */
	struct element *nast; /* Wskażnik na następny obiekt */
	
}element;

/* Czyni operacje na stosie czytelniejszymi */
typedef element * wsk;

/*	Struktura odwzorowująca stos*/
typedef struct stos
{
	int rozmiar;/*Ile el. na stosie */
	wsk glowa;/* Wskaźnik na el. na szczycie stosu */
}stos;


/*	Inicjalizacja nowego stosu 
*	[in,out] *x - nowy stos */
void init_stos(stos *x)
{
	x->rozmiar=0;
	x->glowa=NULL;	
}

/*	Dodaj nowy el do. stosu
*	Jak to na stosie "bywa": nowy element
*	wstawiany jest na górę 
*	Procedura sama tworzy instancje obiektu,
*	z danymi wartościami
*	[in,out] *x- stos do którego dodajemy
*	[in] val, adres - cechy nowego elementu */
void dodaj_stos(stos *x, int val, int adres)
{
	/* Najpierw utwórz nowy element 
	 *	z danymi wartościami.*/ 
	element *nowy;
	nowy=malloc( sizeof (element) );
	nowy->dane.val=val;
	nowy->dane.adres=adres;
	
	/*	Teraz dodaj go do stosu
	 *	i aktualizuj odpowiednie wartości
	 *	dla stosu */ 
	nowy->nast=x->glowa;
	x->rozmiar++;/* Powiększ liczbę elementów */
	x->glowa=nowy;
}

/*	Zwraca wartość ze szczytu stosu.
*	Nie zdejmuje nic ze stosu.
*	[in] x- stos o który pytamy
*	@return wartości ze szczytu stosu*/
wartosci wartosc_stos(stos x)
{
	return x.glowa->dane;
}

/*	Zwraca wartość ze szczytu stosu,
*	a potem zdejmuje element ze stosu.
*	[in,out] x- stos, który edytujemy
*	@return wartości ze szczytu stosu*/
wartosci zdejmij_stos(stos *x)
{
	/*Pamiętaj przed usunięciem */
	wartosci tmp=x->glowa->dane;
	/*Usuń element ze czystu stosu*/
	wsk uchwyt=x->glowa;
	/*Nie trać uchwytu do poprzedniego el.*/
	x->glowa=x->glowa->nast;
	free(uchwyt);
	/*Zmniejsz liczbę el. na stosie*/
	x->rozmiar--;
	
	return tmp;
}
/**@}*/


/**@}*/
/** @name Obsługa maszyny
  @{
 */
/*	Funkcja sprowadzająca wartość W do przedziału
*	[0, N-1].
*	Implementacja zaczerpnięta z treści zadania
*	[in] - W - wartość, która chcemy "unormować"
*	@return int - unormowana wartość W*/
int MOD(int W)
{
	int modulo= W % N_ROZMIAR;
	if(modulo < 0)
		return (modulo + N_ROZMIAR);
	return modulo;	
}


/*	Procedura pisząca w pamięci programu
*	[in] co - wartość jaką wpisujemy
*	[in,out] pamiec - pamieć do której piszemy
*	[in,out] gdzie - pierwsze wolne miejsce w pamięci */   
void pisz_w_pamieci(int co, int *pamiec, int *gdzie)
{
	pamiec[MOD(*gdzie)]=MOD(co);
	*gdzie=MOD(*gdzie+1);/* Przesuń pierwsze wolne miejsce */
}


/** @name Deklaracje interfejsu obsłygi maszyny.
 * 			Dla czytelności kodu oddzieliłem
 * 			część związaną z parsowaniem 
 * 			i działniem maszyny NOC.
 * 			Parsowanie przed main()
 * 			Obsługa maszyny za main().
 */
 
 
 
 
/*	Funkcje potrzebne przy wykonywaniu
*	kodu maszynowego - pozwalają w łatwy
*	sposób przekazać funkcji jakiego
*	"komperartora" ma używać w działaniu */
int rowne(int a, int b)
{
	if(a==b)
		return true;
	return false;	
}

int nierowne(int a, int b)
{
	if(a!=b)
		return true;
	return false;	
}

int wieksze(int a, int b)
{
	if(a>b)
		return true;
	return false;	
}

int mniejsze(int a, int b)
{
	if(a<b)
		return true;
	return false;	
}

int wieksze_rowne(int a, int b)
{
	if(a>=b)
		return true;
	return false;	
}

int mniejsze_rowne(int a, int b)
{
	if(a<=b)
		return true;
	return false;	
}
 
 
 
 
 
 

/*	Któtkie funkcje odpowiadające za
*	działanie arytmetyczne. Myślę, że 
*	nie potrzebują komentarza */

int dodaj (int a, int b) 
{
	return a+b;
}

int odejmij (int a, int b) 
{
	return a-b;
}

int mnoz (int a, int b) 
{
	return a*b;
}

/* Dzielenie całkowite */
int __div (int a, int b) 
{
	if(b!=0)  return a/b ;
	else/* Dzielenie przez ZERO */
	{
		exit(EXIT_FAILURE);
		return -1;
	}
}

/*	Reszta z dzielenia */
int modulo (int a, int b) 
{
	if(b!=0)  return a%b ;
	else/* Dzielenie przez ZERO */
	{
		exit(EXIT_FAILURE);
		return -1;
	}	
}

int nand (int a, int b) 
{
	return ~((a) & (b));
}
/*	Inkrementacja */
int zwieksz (int a) 
{
	return a+1;
}

/*	Dekrementacje */
int zmniejsz (int a) 
{
	return a-1;
}

/*******************************************/
/*	Zebrałem operacje, które mają cechy wspólne
*	i "włożyłem" je do jednej funckcji,
*	która odpowiednio sparametryzowana obsługuje
* 	odpowiednie instrukcje, zgodnie z opisem z zadanie */
	
/* Instrukcje MASZYNY */

void jump_to_subroutine(int* IP, int* AC, int operand_adres);/*0 JSR */
void jump(int operand_adres, int* IP);
void jump_if_ac(int AC, int* IP, int operand_adres, int jak);/*2,3 JPZ, JNZ*/
void wylicz_ac(int* AC, int operand, int (*dzialanie) (int,int));
void zmien_operand(int* operand, int (*dzialanie) (int) );
void wylicz_operand(int AC, int* operand, int (*dzialanie) (int,int));
void skip_if(int AC, int operand, int* IP, int (*warunek) (int,int));
void load_address(int* gdzie, int operand_adres);
void save_val(int* gdzie, int co);

void input_char(int* operand);
void output_char(int operand);
void input_num(int* operand);
void output_num(int operand);

void wykonaj_operacja(int operacja, int *operand, 
			int *operand_adres, int *AC, int *IP, int *SP);
void wykonaj_program(int *pamiec);
/**@}*/


/**@}*/
/** @name Interfejs parsera
  @{
 */
 
int czy_strukt(int co, wzor hash);
int struktura(char *kod, int poz, int *pamiec, int *mem_index,
			stos *okrag, stos *kwadrat);
int komentarz(int poz, char *kod, int dlugosc);
int biale_znaki(int poz, char *kod, int dlugosc);
int liczba(char *kod, int poz, int *pamiec, 
			int *mem_index, int minus_unarny);
int zmienna(char *kod, int poz, int *pamiec,
			int *mem_index, int *etykiety, stos *czekaj);
int dane(char *kod, int poz, int *pamiec, int *mem_index);			
int specjalny(int znak);
int operacja(char *kod, int poz, int *pamiec, 
			int *mem_index, wzor hash, int podwojna);
int backslash(char *kod, int poz, int *pamiec, 
			int *mem_index, stos *okrag, stos *kwadrat, 
			int *etykiety, wzor hash);
int czy_operacja(wzor hash, int znak);
int czy_operand(wzor hash, int znak);
int etykieta_strukturalna(char *kod , int poz,int *pamiec, 
			int *mem_index, stos * okrag, stos *kwadrat);
int tworz_tablica (char *kod, int poz, int *pamiec,
			int *mem_index);
int etykieta_nazwana (char *kod, int poz, 
			int mem_index, int *etykiety);

									
/* 	Procedura parsująca kod.
*	Przechodzi po wszystkich znakach.
*	Analizuje znaki zgodnie z następująca "gramatyką":
* Znak →  <cyfra>  |<  \ >  | < litera>  | <  „ > |  <symobol inny>
* 	[in] kod - kod do analizy
*	[in,out] pamiec do której wpisywane zinterpretowane instr.
*	[in,out] okrag - stos trzymający nawiasy okrągłe
*	[in,out] kwadrat - stos trzymający nawiasy kwadratowe
*	[in,out] czekaj - stos trzymający zmienne, które pojawiają 
* 						się przed swoją deklaracją
*	[in,out] etykiety - tablela symboli, zawiera, pod odpowiednim
* 						indeksem (kod ASCII - kod ASCII(a)
* 						adres zmiennej o danej nazwie
* 	[in] hash - wzory wartości potrzebnych w parsowaniu */	 
void parser(char *kod, int *pamiec, stos *okrag, stos *kwadrat, 
			stos *czekaj, int *etykiety, wzor hash)
{
	/*	Długość kodu */
	int dlugosc = (int) strlen(kod);
	/*	Indeksy */
	int poz=0; int mem_index=0;
	/*	Najpierw omiń wszystkie białe znaki */
	poz+=biale_znaki(poz, kod, dlugosc);
	while(poz<dlugosc)/*Trwaj do konća kodu */
	{
	
		int znak=kod[poz++];/* Aktualnie przetwarzany znak 
								Przesuń potem indeks czytania*/				
		if(znak==KOMENTARZ)/* Omiń komentarz */
			poz+=komentarz(poz, kod, dlugosc);
		else
		{	
			if( isdigit( znak ) )/*Czy cyfra  1.)*/
				poz+=liczba(kod, poz-1, pamiec, 
							&mem_index, false)-1;
			else if(znak==BACKSLAH)/* Backslah 2.) */
				poz+=backslash(kod, poz, pamiec, &mem_index, 
					okrag, kwadrat, etykiety, hash);
			else if (znak==TABLICA)/* Blok danych 3.) */
				poz+=dane(kod, poz, pamiec, &mem_index);
			else if( isalpha( znak ) ) /*Czy litera 4.) */
				poz+=zmienna(kod, poz-1, pamiec, &mem_index, 
						etykiety, czekaj);
			else if( czy_strukt( znak, hash ) ) /* Etykieta str 5.)*/
				poz+=struktura(kod, poz-1, pamiec, &mem_index,
						okrag, kwadrat) - 1 ;
			else /* Symbol operacji 6.)*/
				poz+=operacja(kod, poz-1, pamiec, 
							&mem_index, hash,false) - 1;
		}
		poz+=biale_znaki(poz, kod, dlugosc);
	}	
}

/*	Funckja rozpoznaje czy dany symbol 
*	to etykieta strukturalna
*	[in] co - symbol do analizy
*	[in] hash - wzorzec do którego porównanie 
*	@return true jeśli TAK, wpp false*/
int czy_strukt(int co, wzor hash)
{
	int i=0;
	/*Czy którykolwiek z nawiasów */
	for(i=0;i<NAWIASY;i++)
	{
		if(hash.nawiasy[i]==co)
			return true;/* TAK */
	}
	return false;/* NIE */
}

/*	Funkcja obłsuguje pojawiającą się w	
*	kodzie etykiere strukturalną, nię bedącą deklaracją.
*	Wywoływana tylko w parser.
*	[in] kod - miejsce skąd symbol
* 	[in] poz - pozycja z której bierzemy 
*				symbol do przetworzenia
* 	[in,out] pamiec - miejsce gdzie piszemy
* 			 przetworzony kod maszynowy
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	[int,out] okrag - stos trzymający nawiasy okrągłe
*	[in,out] kwadrat - stos trzymający nawiasy kwadratowe
*	@return - ile wczytanych znaków */
int struktura(char *kod, int poz, int *pamiec, int *mem_index,
			stos *okrag, stos *kwadrat)
{

	wartosci tmp; /* Pomocnicza zmienna */
	
	/*Rozpoznaj jaki nawias */
	
	/*Zamykające*/
	if(kod[poz]==')')
	{
		tmp=zdejmij_stos(okrag);/*Kończy jakąs strkture*/
		/*	Pobierz adres deklaracji str. 
		 *	i wpisz go do pamięci*/
		pisz_w_pamieci(tmp.adres, pamiec, mem_index);
	}
	else if(kod[poz]==']')
	{/*	Analogicznie do powyższego */
		tmp=zdejmij_stos(kwadrat);
		pisz_w_pamieci(tmp.adres, pamiec, mem_index);
	}
	
	/*Otwierające*/
	else if (kod[poz]=='(')
	{
		/*	Etykieta pojawią się przed swoją deklaracją.
		 *	Odłóż ją do czekania, na stos */
		dodaj_stos(okrag,'(',*mem_index);
		*mem_index+=1;
	}
	else
	{/*	Analogicznie do powyższego */
		dodaj_stos(kwadrat,'[',*mem_index);
		*mem_index+=1;
	}
	return 1; /*	Ile wczytanych znaków */
}			

/*	Funkcja omija wszystko co obrębie komentarza
*	[in] kod - miejsce skąd symbol
* 	[in] poz - pozycja z której bierzemy 
*				symbol do przetworzenia
*	[in] dlugosc - dlg. calego kodu
*	@return - ile ominiętych znaków  */
int komentarz(int poz, char *kod, int dlugosc)
{
	int przesun=0;
	/*	Aż do końca komentarza */
	while( poz+przesun<dlugosc && 
			kod[poz+przesun]!=NOWA_LINIA)
		przesun++;
		
	return przesun+1;/*Omin jeszcze znak nowej linii */
}

/*	Funkcja omija wszystkie kolejne białe znaki
*	[in] kod - miejsce skąd symbol
* 	[in] poz - pozycja z której bierzemy 
*				symbol do przetworzenia
*	[in] dlugosc - dlg. calego kodu
*	@returnint zmienna(char *kod, int poz, int *pamiec,
			int *mem_index, int *etykiety, stos *czekaj) - ile ominiętych znaków  */
int biale_znaki(int poz, char *kod, int dlugosc)
{
	int przesun=0;
	int znak=kod[poz+przesun];
	
	while((znak==SPACJA || znak==TABULACJA
			||znak==NOWA_LINIA)
			&&(poz+przesun<dlugosc))
	{
		przesun++;
		/*	Kolejny znak */
		znak=kod[poz+przesun];
	}	
	return przesun;			
}


/*	Funkcja obłsuguje pojawiającą się w	
*	kodzie liczbe.
*	[in] kod - miejsce skąd symbol
* 	[in] poz - pozycja z której bierzemy 
*				symbol do przetworzenia
* 	[in,out] pamiec - miejsce gdzie piszemy
* 			 przetworzony kod maszynowy
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	[int] minus_unarny- jeśli true to liczba z 
* 				minusem unarnym	
* 				wpp false
*	@return - ile wczytanych znaków */
int liczba(char *kod, int poz, int *pamiec, 
			int *mem_index, int minus_unarny) 
{
	int co=0; /* Wczytana liczba */
	int dlg;/* Dlg wczytanej liczby */		
	sscanf(&kod[poz],"%d%n",&co, &dlg);

	
	if(minus_unarny>0)/* Zrób minus */
		co*=(-1);
		
	pisz_w_pamieci(co, pamiec, mem_index);	
	return dlg;
}


/*	Funkcja obłsuguje pojawiającą się w	
*	kodzie etykiere nazwaną, która nie jest deklaracją
*	Wywoływana tylko w parser.
*	[in] kod - miejsce skąd symbol
* 	[in] poz - pozycja z której bierzemy 
*				symbol do przetworzenia
* 	[in,out] pamiec - miejsce gdzie piszemy
* 			 przetworzony kod maszynowy
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	[in,out] etykiety - tabela symboli, gdzie spamietana
* 						adresy już zadeklarowanych etykiet
*	[in,out] czekaj - stos na który wrzucane oczekujące
* 					na swoją deklaracje etykiety
*	@return - ile wczytanych znaków */
int zmienna(char *kod, int poz, int *pamiec,
			int *mem_index, int *etykiety, stos *czekaj)
{

	int nazwa=kod[poz];
	
	/*	Czy etykieta już zadeklarowanae */
	int adres=etykiety[nazwa-A_ZERO];
	if(adres!=-1)/*Jest już taka zmienna zadeklarowana */
		pisz_w_pamieci(adres, pamiec, mem_index);
	else
	{/* Nie ma */
		/* Wrzuć ją do oczekujących */
		dodaj_stos(czekaj, nazwa, *mem_index);	
		*mem_index+=1;
	}
	
	return 0;	
}

/*	Funkcja obłsuguje pojawiające się w	
*	kodzie bloki danych.
*	Wywoływana tylko w parser.
*	[in] kod - miejsce skąd symbol
* 	[in] poz - pozycja z której bierzemy 
*				symbol do przetworzenia
* 	[in,out] pamiec - miejsce gdzie piszemy
* 			 przetworzony kod maszynowy
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	@return - ile wczytanych znaków */
int dane(char *kod, int poz, int *pamiec, int *mem_index)
{
	int przesun=0; int spec;
	int znak=kod[poz];
	
	while(znak!=TABLICA)/* Aż do końca bloku danych */
	{
		if(znak==BACKSLAH && /* Czy znak specjalny */
			(spec=specjalny(kod[poz+przesun+1]))>0) 
		{
			/*Traktuj jako znak specjalny */
			znak=spec;
			przesun++;
		}
		pisz_w_pamieci(znak, pamiec, mem_index);
		przesun++;
		znak=kod[poz+przesun];
	}
	return przesun + 1;/* Jeszcze omin znak końca danych */
}

/*	Pomocnicza funkcji rozpoznająca czy dany symbol
*	tworzy z backslashem znak specjalny
*	[in] znak - znak o któ©y pytamy
*	@return true - jeśli TAK, wpp false */
int specjalny(int znak)
{
	switch(znak)
	{
		case 'a':
			return '\a';
		case 'b':
			return '\b';
		case 'f':
			return '\f';
		case 'n':
			return '\n';
		case 'r':
			return '\r';
		case 't':
			return '\t';
		case 'v':
			return '\v';
		case BACKSLAH:
			return '\\';	
		case TABLICA:
			return '\"';								
	}
	return 0;/* NIE */
}


/*	Funkcja obłsuguje pojawiającą się w	
*	kodzie operacje.
*	Wywoływana tylko w parser.
*	[in] kod - miejsce skąd symbol
* 	[in] poz - pozycja z której bierzemy 
*				symbol do przetworzenia
* 	[in,out] pamiec - miejsce gdzie piszemy
* 			 przetworzony kod maszynowy
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	[in] hash - wzory wartości potrzebnych w parsowaniu 
*	[in] podwojna - true jeśli operacja dwuznakow, wpp false
*	@return - ile wczytanych znaków */
int operacja(char *kod, int poz, int *pamiec, 
			int *mem_index, wzor hash, int podwojna)
{

	int przesun=0;
	int kod_maszynowy=0;
	int symbol=kod[poz];
	
	if(podwojna)
	{/* Dwa znaki */
		kod_maszynowy+=hash.podwojne[symbol]*KODUJ;
		/*BYLO TAK*/
		/*przesun++;*/
	}
	else
		kod_maszynowy+= hash.pojedyncze[symbol]*KODUJ;
		
	przesun++;	
	/*Wczytaj operand z kolejnego znaku*/
	if( czy_operand(hash, kod[poz+1]))
	{/* Operand o kodzie różnym od 5 */
		przesun++;
		kod_maszynowy+=hash.operand[(int)kod[poz+1]];
	}
	else/*	Operand o kodzie 5 */
		kod_maszynowy+=5;

	/*Pisz w pamieci*/
	pisz_w_pamieci(kod_maszynowy, pamiec, mem_index);
	
	return przesun;
}


/* 	Funckja parsująca kod, który rozpoczyna się BACHSLASHA.
* 	[in] kod - kod do analizy
*	[in,out] pamiec do której wpisywane zinterpretowane instr.
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	[in,out] okrag - stos trzymający nawiasy okrągłe
*	[in,out] kwadrat - stos trzymający nawiasy kwadratowe
*	[in,out] czekaj - stos trzymający zmienne, które pojawiają 
* 						się przed swoją deklaracją
*	[in,out] etykiety - tablela symboli, zawiera, pod odpowiednim
* 						indeksem (kod ASCII - kod ASCII(a)
* 						adres zmiennej o danej nazwie
* 	[in] hash - wzory wartości potrzebnych w parsowaniu
*	@return - ile wczytanych znaków */
int backslash(char *kod, int poz, int *pamiec, 
			int *mem_index, stos *okrag, stos *kwadrat, 
			int *etykiety, wzor hash)
{
	int znak=kod[poz]; int przesun=0;
	
		
	if( isalpha( znak ) ) /* Litera 1.) */
		przesun+=etykieta_nazwana(kod, poz, 
					*mem_index, etykiety);
	else if(znak==TABLICA)/* 	Tablica 2.)*/	
		przesun+=tworz_tablica(kod, poz+1,
					pamiec, mem_index);/*+1;*/
	else if( czy_operacja(hash, znak) )/* Operacja 3.) */
		przesun+=operacja(kod, poz, pamiec, 
						mem_index, hash, true);
	else if( isdigit (znak) )/*	Cyfra 4.) */
		przesun+=liczba(kod, poz, pamiec, mem_index, true);	
	else/*	Struktura 5.) */
		przesun+=etykieta_strukturalna(kod, poz, 
					pamiec, mem_index, okrag, kwadrat);
		
	return przesun;	
}

/*	Pomocnicza funkcja rozpoznająca
*	czy dany symbol wraz z backslashem
* 	tworzy operacje
*	[in] hash - wzorzec z symbolami
*	[in] znak - symbol do analizy
*	@return true jeśli TAK, wpp false*/
int czy_operacja(wzor hash, int znak)
{
	if(hash.podwojne[znak]!=-1)
		return true;/* TAK */
	return false;/* NIE */
}

/*	Pomocnicza funkcja rozpoznająca
*	czy dany symbol jest operandem i
*	czy jego kod jest różny od 5(operand pusty).
*	[in] hash - wzorzec z operandami
*	[in] znak - symbol do analizy
*	@return true jeśli TAK, wpp false*/
int czy_operand(wzor hash, int znak)
{
	if(hash.operand[znak]!=-1)
		return true;
	return false;	
}

/* 	Funckja obłsugująca pojawiającą sie kodzie deklaracje
*	etykiety strukturalnej. Wywoływana tylko w backslash ().
* 	[in] kod - kod do analizy
*	[in,out] pamiec do której wpisywane zinterpretowane instr.
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	[in,out] okrag - stos trzymający nawiasy okrągłe
*	[in,out] kwadrat - stos trzymający nawiasy kwadratowe
*	@return - ile wczytanych znaków */
int etykieta_strukturalna(char *kod , int poz,int *pamiec, 
			int *mem_index, stos * okrag, stos *kwadrat)
{
	wartosci tmp;/*	Pomocnicza zmienna */
	
	/* Rozpoznaj nawias */
	
	/* Zamykające */
	if(kod[poz]==')')
	{/*	Nawias deklaracją etykiety która już na stosie */
		
		/*Obsłuż oczekujące etykiete */
		tmp=zdejmij_stos(okrag);
		pamiec[tmp.adres]=*mem_index;
	}
	else if(kod[poz]==']')
	{/*Jak wyżej*/
		tmp=zdejmij_stos(kwadrat);
		pamiec[tmp.adres]=*mem_index;
	}
	
	/* Otwierające */
	/*	Deklaracja nowej struktury */
	else if (kod[poz]=='(')
		dodaj_stos(okrag,'(',*mem_index);
	else
		dodaj_stos(kwadrat,'[',*mem_index);

	return 1;
}


/* 	Funckja obłsugująca pojawiającą sie kodzie deklaracje
*	etykiety nazwanej. Wywoływana tylko w backslash ().
* 	[in] kod - kod do analizy
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	[in,out] etykiety - tabela symboli, gdzie spamietana
* 			adresy już zadeklarowanych etykiet
*	@return - ile wczytanych znaków */
int etykieta_nazwana (char *kod, int poz,
			int mem_index, int *etykiety)
{
	/*	Deklaracja nowej zmiennej, 
	 * dodaj ją do tablicy symboli */
	etykiety[kod[poz]-A_ZERO]=mem_index;
	return 1;
}


/* 	Funckja obłsugująca pojawiającą sie kodzie tablicc.
*	Wywoływana tylko w backslash ().
* 	[in] kod - kod do analizy
*	[in,out] pamiec do której wpisywane zinterpretowane instr.
*	[in,out] mem_index - pierwsze wolne miejsce w pamięci
*	@return - ile wczytanych znaków */
int tworz_tablica (char *kod, int poz, int *pamiec,
			int *mem_index)
{
	int i, co, dlg;	
		
	/* Wczytaj rozmiar tablicy */	
	sscanf(&kod[poz],"%d%n",&co,&dlg);
	/*Wypełnij kolejne komórki pamięci ZERAMI */
	for(i=0;i<co;i++)
		pisz_w_pamieci(0, pamiec, mem_index);

	return dlg+1;
}


/*	Procedura definiująca "stałe".
*	Opisuje wszystkie potrzebne w procesie parsowanie 
*	wartości. Wzroce zaczerpnięte z treści zadanie
*	[in,out] hash- obiekt przechowujący wzorce */
void init_wzorzec(wzor *hash)
{
	int i; /* Iterator */
	/*	Najpierw poproś o pamięć*/
	hash->pojedyncze= (int*) malloc( sizeof(int) *MAX_ASCII ) ;
	hash->operand= (int*) malloc( sizeof(int) *MAX_ASCII ) ;
	hash->podwojne= (int*) malloc( sizeof(int) *MAX_ASCII ) ;
	hash->nawiasy= (int*) malloc( sizeof(int) *NAWIASY ) ;
	
	/*	Początkowa inicjalizacja */
	for(i=0;i<MAX_ASCII;i++)
	{
		hash->pojedyncze[i]=-1;
		hash->podwojne[i]=-1;
		hash->operand[i]=-1;
	}
	
	/*	Kody operacji bez backslasha */ 			
	hash->podwojne['_']=0;
	hash->podwojne['~']=3;
	hash->podwojne['|']=5;
	hash->podwojne['}']=6;
	hash->podwojne['{']=7;
	hash->podwojne['+']=8;
	hash->podwojne['-']=9;
	hash->podwojne['*']=10;
	hash->podwojne['/']=11;
	hash->podwojne['%']=12;
	hash->podwojne['=']=13;
	hash->podwojne['<']=14;
	hash->podwojne['>']=15;
	hash->podwojne['?']=30;
	hash->podwojne['!']=31;
	
	/*	Kody operacji z backslashem */ 
	hash->pojedyncze['_']=1;
	hash->pojedyncze['~']=2;
	hash->pojedyncze['|']=4;
	hash->pojedyncze['+']=16;
	hash->pojedyncze['-']=17;
	hash->pojedyncze['*']=18;
	hash->pojedyncze['/']=19;
	hash->pojedyncze['%']=20;
	hash->pojedyncze['=']=21;
	hash->pojedyncze['<']=22;
	hash->pojedyncze['>']=23;
	hash->pojedyncze[',']=24;
	hash->pojedyncze['`']=25;
	hash->pojedyncze['.']=26;
	hash->pojedyncze[':']=27;
	hash->pojedyncze['?']=28;
	hash->pojedyncze['!']=29;
	
	/*	Kody operandów */ 
	hash->operand['@']=0;
	hash->operand['^']=1;
	hash->operand['}']=2;
	hash->operand['{']=3;
	hash->operand['#']=4;
	hash->operand['$']=6;
	hash->operand['&']=7;
	
	/*	Obsługiwane etykiety strukturalne */ 
	hash->nawiasy[0]='(';
	hash->nawiasy[1]=')';
	hash->nawiasy[2]='[';
	hash->nawiasy[3]=']';
}

/*	Procedura czyści pamięć,
*	wywoływana przed rozpoczęciem
*	parsowanie
*	[in,out] pamięć - miejsce gdzie czyścimy*/
void init_pamiec(int *pamiec)
{
	int i;
	for(i=0;i<N_ROZMIAR;i++)
	{
		pamiec[i]=0;
	}
}

/*	Porcedura wywływana po przejrzeniu całego kodu.
*	Obsługuje etykiety nazwane, które pojawiły się przed
*	swoją deklaracją. Z treści zadania wynika, 
*	że kod jest poprawny, stąd zakłądam,
*	że dla każdej zmiennej pojawi się deklaracja.
*	[in,out] etykiety - tabela symboli, gdzie spamietana
* 			adresy już zadeklarowanych etykiet
*	[in,out] czekaj - stos trzymający zmienne, które pojawiają 
* 						się przed swoją deklaracją
*	[in,out] pamiec do której wpisywane adresy etykiet */
void adresuj_zmienne(int *etykiety, stos *czekaj, int * pamiec)
{
	int i=0; int rozmiar=czekaj->rozmiar;
	/*	Przetwórz cały stos */
	for(i=0;i<rozmiar;i++)
	{
		/*	Zdejmuj po kolei elementy ze stosu*/
		wartosci tmp;
		tmp=zdejmij_stos(czekaj);
		/*	Obsłuż czekającą etykiete */
		pamiec[tmp.adres]=etykiety[tmp.val-A_ZERO];	
	}
}

/*	Procedura obsługije inicjalizcje wszystkich potrzebnych
*	struktur danych*/
void init_poczatek(int *etykiety, stos *okrag,  stos *kwadrat,  
				stos *czekaj, int *pamiec)
{
	int i;
	for(i=0;i<'z'-'a'+1;i++)
			etykiety[i]=-1;
			
	init_stos(okrag);
	init_stos(kwadrat);
	init_stos(czekaj);

	init_pamiec(pamiec);								
}

/*******************************************************************
 * ***************************************************************
 * ********************************************************
 * ***************************************************
 * *********************************************
 * ****************************************
 */ 
 
int main(int argc, char *argv[])
{	

	if(argc>1)
	{/*	Jako argument przekazano kod */
		
		/*	Deklaracje "globalne" */
		wzor hash;/* Wzorcowe wartości */
		int etykiety['z'-'a'+1]; /* Tabela symboli */
		stos okrag;/* Nawiasy okrągłe*/
		stos kwadrat;/* Nawiasy kwadratowe */
		/* Zmienne pojawiające się 
		 * przed swoją deklaracją*/
		stos czekaj;
		
		/*	Pamięć maszyny NOC */
		int pamiec[N_ROZMIAR];


		/*	Początkowa inicjalizacja */
		init_poczatek(etykiety, &okrag, 
					&kwadrat, &czekaj, pamiec);			
		init_wzorzec(&hash);
		
		/*	Parsuj dany kod */
		parser(argv[1], pamiec, &okrag, &kwadrat, &czekaj, 
				etykiety, hash);
		
		/*	Sprzątaj na koniec parsowania */		
		adresuj_zmienne(etykiety, &czekaj, pamiec);		


		/*	Wykonaj kod maszynowy programu */
		wykonaj_program(pamiec);
	}
	return 0;
}


/****************
 * *****************
 * ********************
 * ***********************
 * ****************************
 * **********************************
 * **************************************
 * *******************************************
 * ***********************************************************/

/*	Funkcja odpowiadająca za jump_to_subroutine
*	0 	\_ JSR
*	zapisuje IP do AC a następnie adres operandu do IP */
void jump_to_subroutine(int *IP, int *AC, int operand_adres)
{
	(*AC)=(*IP);
	(*IP)=MOD(operand_adres);
}

/*	Funkcja odpowiadająca za jump
*	1 	_ 	JMP
*	zapisuje adres operandu do IP */
void jump(int operand_adres, int *IP)
{
	(*IP)=MOD(operand_adres);
}

/*	Funkcja odpowiada za:
*	a.) Jeśli jak = ROWNE : jump if AC is zero
* 			2 	~ 	JPZ
* 	b.) Jeśli jak = NIEROWNEROWNE : 
* 						jump if AC not is zero
* 			3 	\~ 	JNZ */
void jump_if_ac(int AC, int *IP, int operand_adres, int jak)
{
	switch(jak)
	{
		case ROWNE:/* a.) */
			if(AC==0)
				(*IP)=MOD(operand_adres);
		break;
		case NIEROWNE:/* b.)*/
			if(AC!=0)
				(*IP)=MOD(operand_adres);
		break;	
	}
}


/*	Odpowiada za:
*	a.)	nand		4 	| 	NND
* 			liczy NAND wartości AC i 
* 			wartości operandu, 
* 			wynik zapisuje w AC
*	b.) add 		16 	+
* 			dodaje wartość operandu do
* 			wartości AC 
* 			i zapisuje wynik w AC
* 	c.) subtract	17 	-	
* 			odejmuje wartość operandu 
* 			od wartości AC i zapisuje 
* 			wynik w AC
* 	d.) multiply	18 	*
* 			mnoży wartość AC przez 
* 			wartość operandu i
* 			zapisuje wynik w AC
* 	e.) divide		19 	/
* 			dzieli wartość AC przez 
* 			wartość operandu i 
* 			zapisuje wynik w AC
* 	f.) modulo		20 	%
* 			liczy resztę z dzielenia wartości
* 			 AC przez wartość operandu i 
* 				zapisuje wynik w AC
* 	Która opercja będzie wykonana zależy 
* 	od przekazanego wsk. na fun.
* 	[in,out] AC- wartość operandu
* 	[in] operand - wartość operandu
*	[in] (*dzialanie) - wsk. na fun */
void wylicz_ac(int *AC, int operand, int (*dzialanie) (int ,int))
{
	int wynik=dzialanie(*AC,operand);
	(*AC)=MOD(wynik);
}

/*	Odpowiada za:
*	a.) increment \} 6 zwiększa o 1 wartość operandu
* 	b.) decrement \{ 7 zmniejsza o 1 wartość operandu
*	Rozróżnienie odbywa się za pomocą wsk. na fun.
* 	(*dzialanie), ktory zwraca albo powiekszony 
* 	albo pomniejszony argument
*  	[in,out] operand - wartość operandu
* 	[in] (*dzialanie) - wsk. na fun*/
void zmien_operand(int *operand, int (*dzialanie) (int) )
{
	int wynik=dzialanie(*operand);
	(*operand)=MOD(wynik);
}


/*	Odpowiada za:
*	a.) reverse nand 		5 	\| 	DNN
* 			liczy NAND wartości operandu i wartości AC, 
* 			wynik zapisuje w operandzie
* 	b.) reverse add 8 		8 	\+ 	DDA
* 			dodaje wartość AC do wartości operandu i 
* 			zapisuje wynik w operandzie
* 	c.) reverse subtract	9 	\- 	BUS
* 			odejmuje wartość AC od wartości operandu 
* 			i zapisuje wynik w operandzie
* 	d.) reverse multiply	10 	\* 	LUM
* 			mnoży wartość operandu przez wartość AC 
* 			i zapisuje wynik w operandzie
* 	e.) reverse divide  	11 	\/ 	VID
* 			dzieli wartość operandu przez wartość AC 
* 			i zapisuje wynik w operandzie
* 	f.) reverse modulo		12 	\% 	DOM
* 			liczy resztę z dzielenia wartości operandu 
* 			przez wartość AC i zapisuje wynik w operandzie
*	Która opercja będzie wykonana zależy od przekazanego wsk.
* 	na fun.
* 	[in] AC- wartość operandu
* 	[in,out] operand - wartość operandu
*	[in] (*dzialanie) - wsk. na fun */
void wylicz_operand(int AC, int *operand, int (*dzialanie) (int,int))
{
	int wynik=dzialanie(*operand,AC);
	(*operand)=MOD(wynik);
}

/*	Opowiada za:
*	a.) skip if not equal 			13 \= 	SNE
* 	b.) skip if greater or equal 	14 	\< 	SGE
* 	c.) skip if less or equal		15 	\> 	SLE
* 	d.) skip if equal				21 	= 	SEQ
* 	e.)	skip if less then			22 	< 	SLT
* 	f.) skip if greater then		23 	> 	SGT 
* 	[in] AC - wartość AC
* 	[in] operand - wartość operandu
* 	[in,out] IP - wskażnik instrukcji
* 	[in] (*warunek) - wskaźnik na funkcje, bedącą
* 			"komperatorem" pozwalającym obsłużyć wszystkie
* 			wymienione operacje (np. rowne(a,b)) */
void skip_if(int AC, int operand, int* IP, int (*warunek) (int,int))
{
	if(warunek(AC,operand))
	{
		int wynik=(*IP)+2;
		(*IP)=MOD(wynik);
	}
}


/*	Procedura odpowiada za:
*	a.)  zapisuje w AC adres operandu 24 LAA ,
*	b.)	 zapisuje w IP adres operandu 25 LAS `
*	W zależności od gdzie zleży zachowanie procedury
* 	[in,out] gdzie - miejsce gdzie piszemy
* 					IP lub AC
* 	[in] operand_adres - adres do zapisania*/ 
void load_address(int *gdzie, int operand_adres)
{
	(*gdzie)=MOD(operand_adres);
}

/*	Procedura odpowiada za:
*	a.)  zapisuje w AC wartość operandu 26 LDA .
*	b.)	 zapisuje wartość AC w operandzie 27 STA :
*	W zależności od gdzie zależy zachowanie proceudry
* 	[in,out] gdzie - miejsce gdzie piszemy
* 					AC lub inny adres
* 	[in] operand_adres - adres do zapisania (może być AC)*/ 
void save_val(int *gdzie, int co)
{
	(*gdzie)=MOD(co);
}

/*	Odpowiada za :  28 ICH ? input character
*	Próbuje wczytać znak z wejścia. 
*	Jeśli się uda, zapisuje w operandzie jego kod, 
*	jeśli nie, wpisuje tam N - 1. 
*	[in,out] operand - operand do kotrego piszemy */
void input_char(int *operand)
{
	int znak=getchar();
	if (ferror (stdin))/*Nie udało się */
		(*operand)=N_ROZMIAR-1;
	else
		(*operand)=MOD(znak);
}

/* 	Odpowiada za: OCH 29 ! output character
*	Wypisuje na wyjście znak, 
*	którego kod jest wartością operandu.
* 	[in] operand którego wartość bierzemy */
void output_char(int operand)
{
	printf("%c",operand);
}

/*	Odpowiada za :  30 INU  \? input number
*	Próbuje wczytać znak z wejścia. 
*	próbuje wczytać z wejścia zapisaną
*  	dziesiętnie liczbę całkowitą. 
* 	Jeśli się uda, zapisuje ją w operandzie, 
* 	jeśli nie, wpisuje tam N - 1.
*	[in,out] operand - operand do kotrego piszemy */
void input_num(int *operand)
{
	
	int num;
	scanf("%d",&num);
	if (ferror (stdin))/*Nie udało się */
		(*operand)=N_ROZMIAR-1;
	else
		(*operand)=num;
}

/* 	Odpowiada za: 31 ONU  \! output number 
*	wypisuje na wyjście, w zapisie dziesiętnym, 
* 	wartość operandu. Poza jej cyframi nie 
* 	są wypisywane żadne inne znaki, nawet spacje.
* 	[in] operand którego wartość bierzemy */
void output_num(int operand)
{
	printf("%d",operand);
}


/*	Procedura wywłouje opdowiednią
*	instrukcje, na podstawie otrzymanych parametrów
*	[in] operacja - numer operacji do wykonania
*	[in,out] operand instrukcji
* 	[in,out] operand_adres - adres operandu instrukcji
*	[in,out] AC - akumulator
*	[in,out] IP - wskaźnik instrukcji
*	[in,out] SP - wskaźnik stosu*/
void wykonaj_operacja(int operacja, int *operand, 
			int *operand_adres, int *AC, int *IP, int *SP)
{
	/*	Szczegółowy opis każdej opercji przy definicji*/
	switch(operacja)
	{	
		case 0:
			jump_to_subroutine(IP, AC, *operand_adres);
		break;
		
		case 1:
			jump(*operand_adres, IP);
		break;
			
		case 2:
			jump_if_ac(*AC, IP, *operand_adres, ROWNE);
		break;
		
		case 3:
			jump_if_ac(*AC, IP, *operand_adres, NIEROWNE);
		break;
		
		case 4:
			wylicz_ac(AC, *operand, nand);
		break;
		
		case 5:
			wylicz_operand(*AC, operand, nand);
		break;
		
		case 6: 
			zmien_operand(operand, zwieksz);
		break;
		
		case 7:
			zmien_operand(operand, zmniejsz);
		break;
		
		case 8:
			wylicz_operand(*AC,  operand,  dodaj);
		break;
		
		case 9:
			wylicz_operand(*AC,  operand,  odejmij);
		break;
			
		case 10:
			wylicz_operand(*AC,  operand,  mnoz);
		break;
		
		case 11:
			wylicz_operand(*AC,  operand, __div);
		break;
		
		case 12:
			wylicz_operand(*AC,  operand, modulo);
		break;
		
		case 13:
			skip_if(*AC, *operand, IP, nierowne);
		break;
		
		case 14:
			skip_if(*AC, *operand, IP, wieksze_rowne);
		break;
		
		case 15:
			skip_if(*AC, *operand, IP, mniejsze_rowne);
		break;
		
		case 16: 
			wylicz_ac(AC, *operand, dodaj);
		break;
		
		case 17:
			wylicz_ac(AC, *operand, odejmij);
		break;
		
		case 18:
			wylicz_ac(AC, *operand, mnoz);
		break;
		
		case 19:
			wylicz_ac(AC, *operand, __div);
		break;
		
		case 20:
			wylicz_ac(AC, *operand, modulo);
		break;	
		
		case 21:
			skip_if(*AC, *operand, IP, rowne);
		break;
		
		case 22:
			skip_if(*AC, *operand, IP, mniejsze);
		break;
		
		case 23:
			skip_if(*AC, *operand, IP, wieksze);
		break;
		
		case 24:
			load_address( AC, *operand_adres);
		break;
			
		case 25:
			load_address( SP, *operand_adres);
		break;
		
		case 26:
			save_val(AC, *operand);
		break;	
		
		case 27:
			save_val(operand, *AC);
		break;	
		
		case 28:
			input_char(operand);
		break;
		
		case 29:
			output_char(*operand);
		break;
		
		case 30:
			input_num(operand);
		break;
		
		case 31:
			output_num(*operand);
		break;
	
	}
}

/*	Procedura wykonująca kod maszynowy.
*	Pełni również funkcje rejestru instrukcji,
*	o którym wspomniano w zadaniu.
*	"Serce" maszyny noc, to tutaj rozpoznaje
* 	to tutaj symulowana jest cała pętla
*	działania programu.
*	Procedura czyta po kolei program
*	z wcześniej sparsowanego kodu, który
*	przekazany jest w pamięci.
*	Rozpoznaje numer operacji i numer operandu.
*	Następnie wywołuje procedurą wykonującą
* 	operacje, z odpowiednimi parametrami
*	[in,out] pamiec- pamieć maszyny NOC */
void wykonaj_program(int *pamiec)
{
	
	int akumulator=0;
	int wsk_stos=0;
	int wsk_instr=0;
	
 	/*Dopóki nie koniec instrukcji */
	while(pamiec[wsk_instr]!=0)
	{
		/* Słowo maszyny */
		int val=pamiec[wsk_instr];
		
		/*Wylicz kody operacji i operandu */
		int operacja=  val / KODUJ;/* Kod operacji */
		int op= val % KODUJ;/* Kod operandu */
		int operand_adres=0;/*Adres wskaznika w pamieci */
		int *operand=NULL;/* Wskaźnik na operand */
		operand=operand;
		/*	Jaki operand*/
		switch(op)
		{
			/* 	0 	@ 	acc 	
			 * accumulator 	akumulator: 
			 * operandem jest AC*/
			case 0:
				operand=&akumulator;
				wsk_instr=MOD(wsk_instr+1);
			break;
			
			/*	1 	^ 	ind 	
			 * indirect 	
			 * adresowanie pośrednie: 
			 * operandem jest słowo o adresie, 
			 * który jest w AC*/
			case 1:
				operand=&pamiec[akumulator];
				operand_adres=MOD(akumulator);
				wsk_instr=MOD(wsk_instr+1);
			break;
				
			/*	2 	} 	pop 	
			 * postincrement (pop) 	
			 * adresowanie stosowe zwiększające po: 
			 * operandem jest słowo o adresie, 
			 * który jest w SP. 
			 * Dodatkowo, już po określeniu operandu, 
			 * wartość SP jest zwiększana o 1.	*/
			case 2:
				operand=&pamiec[MOD(wsk_stos)];
				wsk_stos=MOD(wsk_stos+1);
				operand_adres=MOD(wsk_stos);
				wsk_instr=MOD(wsk_instr+1);
			break;
				
			/*	3 	{ 	psh 	
			 * predecrement (push) 	
			 * adresowanie stosowe 
			 * zmniejszające przed: 
			 * przed określeniem operandu 
			 * wartość SP jest zmniejszana o 1. 
			 * Następnie jako operand jest 
			 * wskazywane słowo, 
			 * którego adres jest w SP.	*/
			case 3:
				wsk_stos=MOD(wsk_stos-1);
				operand=&pamiec[MOD(wsk_stos)];
				operand_adres=MOD(wsk_stos);
				wsk_instr=MOD(wsk_instr+1);
			break;
				
			/*	4 	# 	imm  
			 * immediate 	
			 * adresowanie natychmiastowe: 
			 * operandem jest argument instrukcji, 
			 * czyli słowo w pamięci 
			 * bezpośrednio za instrukcją */	
			case 4:
				operand= &pamiec[MOD(wsk_instr+1)];
				operand_adres=MOD(wsk_instr+1);
				wsk_instr=MOD(wsk_instr+2);
			break;
			
			/*	5 	(brak) 	abs 	
			 * absolute 	
			 * adresowanie bezwzględne: 
			 * operandem jest słowo, 
			 * którego adres jest wartością 
			 * argumentu instrukcji	*/
			case 5:
				operand= &pamiec[MOD(pamiec[wsk_instr+1])];
				operand_adres=MOD(pamiec[wsk_instr+1]);
				wsk_instr=MOD(wsk_instr+2);
			break;
				
			/*	6 	$ 	dis 	
			 * displacement 	
			 * adresowanie z przesunięciem 
			 * względem SP: 
			 * operandem jest słowo, 
			 * którego adres jest sumą wartości 
			 * argumentu i wartości SP */	
			case 6:
				operand=&pamiec[
							MOD(pamiec[wsk_instr+1]+wsk_stos)];
				operand_adres=MOD(pamiec[wsk_instr+1]+wsk_stos);		
				wsk_instr=MOD(wsk_instr+2);	
			break;
			
			/*	7 	& 	rel 	
			 * relative 	
			 * adresowanie z przesunięciem 
			 * względem IP: 
			 * operandem jest słowo, 
			 * którego adres jest sumą 
			 * wartości argumentu 
			 * i adresu następnego słowa 
			 * za argumentem. */			
			case 7:
				operand=&pamiec[
							MOD(pamiec[wsk_instr+1]+wsk_instr+2)];
				operand_adres=MOD(pamiec[wsk_instr+1]+wsk_instr+2);			
				wsk_instr=MOD(wsk_instr+2);	
			break;	
		}
		
		
		/*	Sprawdź czy :
		 * Operacje, które korzystają z adresu operandu, 
		 * a więc JSR : 0, JMP : 1, JPZ : 2, 
		 * JNZ : 3, LAA :24, LAS :25
		 * nie mogą być wykonane w trybie adresowania acc. 
		 * Próba wykonania instrukcji łączącej 
		 * taką operację z trybem acc powoduje 
		 * zatrzymanie maszyny i przekazanie 
		 * do systemu operacyjnego kodu wyjścia 
		 * EXIT_SUCCESS (czyli wykonujemy exit(EXIT_SUCCESS);).
		 * UWAGA! Z premedytacją używam liczb jako kodu -
		 * myślę, że dodatkowe define tylko przedłużą kod,
		 * akurat w tej sytuacji, a nie wpłyną w dużym
		 * stopniu na bezpieczeństwo kodu*/
		if((operacja == 0 || operacja ==1 || operacja==2 ||
			operacja == 3 || operacja==24 || operacja==25)
			&&(operand==0))
				exit(EXIT_SUCCESS);
		
		wykonaj_operacja(operacja, operand, &operand_adres,
						&akumulator, &wsk_instr, &wsk_stos);
	}

}	
