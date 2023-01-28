#include<msp430x14x.h>
#include "lcd.h"
#include "portyLcd.h"
#include <time.h>
#include <stdlib.h>

unsigned int i=0;
unsigned int sekundy= 0;
unsigned int minuty = 0;
unsigned int godziny = 0;
unsigned int licznik=0;
unsigned int milisekundy = 0;

unsigned int minAl1 = 0;
unsigned int godzAl1 = 0;
unsigned int minAl2 = 0;
unsigned int godzAl2 = 0;
unsigned int flag = 0;
unsigned int highscoreScore[] = {0, 0, 0, 0, 0, 0, 0, 0};
char highscoreName[8][4] = {"", "", "", "", "", "", "", ""};
unsigned int howMany = 0;
unsigned int maxMany = 8;
char linia1[16] = {0,' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
char linia2[16] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
unsigned int indexLinii = 0;
unsigned int punkty = 0;

void wyswietl(char *napis, int linia, int pozycja)
{
	if(linia==2) SEND_CMD(DD_RAM_ADDR2+pozycja);
	else if(linia==1) SEND_CMD(DD_RAM_ADDR+pozycja);
	int i=0;
	while(napis[i] != 0)
	{
		SEND_CHAR(napis[i]);
		i++;
	}
}

void wyswietlGre(char linia1[], char linia2[]){
	SEND_CMD(DD_RAM_ADDR);
	for(int i=0; i<16; i++)
	{
		SEND_CHAR(linia1[i]);
	}
	SEND_CMD(DD_RAM_ADDR2);
	for(int i=0; i<16; i++)
	{
		SEND_CHAR(linia2[i]);
	}
}

void zdefiniujZnaki(){
	SEND_CMD(CG_RAM_ADDR);
        //kura
	char samolot[8] = {0x0,0x2,0x3,0xE,0x1E,0x4,0xA,0x0};
	for(int i =0; i<8; i++)  SEND_CHAR(samolot[i]);

        //lis
	char wrog[8] = {0x0, 0xA, 0x1F, 0x15, 0xE, 0x4, 0x0, 0x0};
	for(int i =0; i<8; i++)  SEND_CHAR(wrog[i]);
        
        //animacja zderzenia dalem tam czaszke
	char wybuch[8] = {0x1E, 0x15, 0x1F,0xA ,0xA , 0xA, 0xA,0xA };
	for(int i =0; i<8; i++)  SEND_CHAR(wybuch[i]);
        
        //wstawienie jajka, na razie nie dziala z niczym innym
	char jajko[8] = {0x0,0x4 ,0xA , 0x11,0x11 ,0x11 ,0xA ,0x4 };
	for(int i =0; i<8; i++)  SEND_CHAR(jajko[i]);
}

void resetujGre()
{
	punkty = 0;
        licznik = 0;
	indexLinii = 0;
	linia1[0] = 0;
	linia2[0] = ' ';
	for(int i=1; i<16; i++)
	{
		linia1[0] = ' ';
		linia2[0] = ' ';
	}
}

const char* UstawNick(){
	clearDisplay();
        int iter = 0;
	char opis[] = {'A','A','A',0};
	wyswietl(opis,1,0);
	wyswietl("-",2,0);
	short int flag1 = 0;
	short int flag2 = 0;
	short int flag3 = 0;
	short int flag4 = 0;
	while(1){

		if(((P4IN & BIT4) == 0)&&(flag1 == 1)){
                        flag1 = 0;
                        clearDisplay(); 
                        opis[iter]++;
                        opis[iter] = (opis[iter]-'A')%26 + 'A';
		}

		if(((P4IN & BIT5) == 0)&&(flag2 == 1)){
			 flag2 = 0;
                        clearDisplay(); 
                        opis[iter]--;
                        opis[iter] = (opis[iter]-'A'+26)%26 + 'A';
		}

		
		if(((P4IN & BIT6) == 0)&&(flag3 == 1)){
			flag3 = 0;
                        clearDisplay();
                        iter++;
                        iter %= 3;
		}
	        
		if((P4IN & BIT4) != 0)flag1=1;
		if((P4IN & BIT5) != 0)flag2=1;
		if((P4IN & BIT6) != 0)flag3=1;
		if((P4IN & BIT7) != 0)flag4=1;
	        
	 
		if(((P4IN & BIT7) == 0)&&(flag4 == 1)){
			clearDisplay();
			break;
		}
                wyswietl(opis,1,0);
	        wyswietl("-",2,iter);
	}
        return opis;
}

int Gra(char linia1[], char linia2[], int LMP3_bits) {
    _BIS_SR(LPM3_bits); 
    if((P4IN & BIT4) ==0) {
        if(linia1[0] == 1) {
            linia1[0] = 2;
            linia2[0] = ' ';
            wyswietlGre(linia1, linia2);
            return 1;
        }
        linia2[0] = ' ';
        linia1[0] = 0;
        indexLinii=0;
    }
    if((P4IN & BIT5) ==0){
	if(linia2[0] == 1) {
            linia2[0] = 2;
            linia1[0] = ' ';
            wyswietlGre(linia1, linia2);
            return 1;
        }
        linia1[0] = ' ';
        linia2[0] = 0;
        indexLinii=1;
    }
    wyswietlGre(linia1, linia2);
	
    for(int i=0; i<15; i++) {
        linia1[i] = linia1[i+1];
        linia2[i] = linia2[i+1];
        linia1[i+1] = ' ';
        linia2[i+1] = ' ';
    }
    if(indexLinii ==0) {
        if(linia1[0] == 1) {
	    linia1[0] = 2;
            linia2[0] = ' ';
            wyswietlGre(linia1, linia2);
            return 1;
        }
        linia1[0] = 0;
    }
    if(indexLinii ==1) {
        if(linia2[0] == 1) {
            linia2[0] = 2;
            linia1[0] = ' ';
            wyswietlGre(linia1, linia2);
            return 1;
        }
        linia2[0] = 0;
    }           
    if(licznik%5 == 0) {
	  punkty++;
	  P2OUT ^=BIT1;
      licznik=1;
      int losowa = rand()%2;
      if(losowa == 0) linia1[15] = 1;
      else linia2[15] = 1;
    }
    return 0;
}

void sortuj(int howMany) {
  if (howMany > 1) {
    for (int i=0; i<howMany-1; i++) {
      int maxi = 0;
      int maxiIndex = -1;
      for (int j=i; j<howMany; j++) {
        if (highscoreScore[j] > maxi) {
             maxi = highscoreScore[j];
             maxiIndex = j;
        }
      }
      
      for (int j=0; j<4; j++) {
        char tt = highscoreName[maxiIndex][j];
        highscoreName[maxiIndex][j] = highscoreName[i][j];
        highscoreName[i][j] = tt;
          
      }
      int t = highscoreScore[i];
      highscoreScore[i] = maxi;
      highscoreScore[maxiIndex] = t;
    }
  }
}

void GraEngine(int LMP3_bits) {
  for (int gh=0; gh<16; gh++) {
    linia1[gh] = ' ';
    linia2[gh] = ' ';
  }
  linia1[0] = 0;
	zdefiniujZnaki();
	srand(time(NULL));
	for(;;) {
		if(Gra(linia1, linia2, LMP3_bits) == 1) {
			break;
		}
	}
	for(int i = 0; i<50; i++) Delayx100us(254);
	const char* t = UstawNick();
	int setIndex = howMany;
	if (howMany >= maxMany) {
		setIndex = 7;
                howMany--;
	}
	strcpy(highscoreName[setIndex], t);
	highscoreScore[setIndex] = punkty;
        howMany++;
        sortuj(howMany);
	resetujGre();
	clearDisplay();
}

void Opis(){
  	clearDisplay();
  	char* opis[] = {"P4 aby cofnac","P1-gora, P2-dol","Nasza gra polega","na omijaniu","przeszkod przy",
	  "pomocy dwoch","przyciskow.","Gra liczy i","zapisuje punkty","kazdego gracza","i jego nick"};
  	wyswietl(opis[0],1,0);
  	wyswietl(opis[1],2,0);
  	int i=1;
  	short int flag1 = 0;
  	short int flag2 = 0;
  	short int flag3 = 0;
  	short int flag4 = 0;
	while(1){
	  
	   	if((P4IN & BIT4) != 0)flag1=1;
	 	if((P4IN & BIT5) != 0)flag2=1;
	 	if((P4IN & BIT6) != 0)flag3=1;
	 	if((P4IN & BIT7) != 0)flag4=1;
	  
		if(((P4IN & BIT4) == 0)&&(flag1 == 1)){
		  	flag1 = 0;
			if(i==0) continue;
			clearDisplay();
			wyswietl(opis[i-1],1,0);
  			wyswietl(opis[i],2,0);
			i--;
		}

		if(((P4IN & BIT5) == 0)&&(flag2 == 1)){
	    	flag2 = 0;
			if(i==10) continue;
			clearDisplay();
			wyswietl(opis[i],1,0);
  			wyswietl(opis[i+1],2,0);
	  		i++;
		}

		if(((P4IN & BIT7) == 0)&&(flag4 == 1)){
			clearDisplay();
	   		break;
		}
	}
}

void Autorzy(){
	clearDisplay();
	char* opis[] = {"P4 aby cofnac","P1-gora, P2-dol","Autorzy:","Dawid Kobeszko","Jakub Komarewski",
	  "Jakub Orlowski"};
	wyswietl(opis[0],1,0);
	wyswietl(opis[1],2,0);
	int i=1;
	short int flag1 = 0;
	short int flag2 = 0;
	short int flag3 = 0;
	short int flag4 = 0;
	while(1){

		if((P4IN & BIT4) != 0)flag1=1;
		if((P4IN & BIT5) != 0)flag2=1;
		if((P4IN & BIT6) != 0)flag3=1;
		if((P4IN & BIT7) != 0)flag4=1;
	  
		if(((P4IN & BIT4) == 0)&&(flag1 == 1)){
			flag1 = 0;
			if(i==0) continue;
			clearDisplay();
			wyswietl(opis[i-1],1,0);
			wyswietl(opis[i],2,0);
			i--;
		}

		if(((P4IN & BIT5) == 0)&&(flag2 == 1)){
			flag2 = 0;
			if(i==5) continue;
			clearDisplay();
			wyswietl(opis[i],1,0);
			wyswietl(opis[i+1],2,0);
			i++;
		}

		if(((P4IN & BIT7) == 0)&&(flag4 == 1)){
			clearDisplay();
			break;
		}
	}
}

char* concat(int it) {
	static char arr[16];
	int len=0;
	for(int i=0; i<16; i++) {
		arr[i] = 0;
	}
	for(int i=0; i<3; i++) {
		arr[i] = highscoreName[it][i];
	}
	arr[3] = ':';
	int num = highscoreScore[it];
	int iter = 4;
	int i = num;
	while(i>0) {
		len++;
		i/=10;
	}
	i = num;
	if(i == 0) {
		arr[iter++] = 0 + '0';
	}
	int tempor = len;
	while(i>0) {
		arr[iter+len-- -1] = (i%10)+'0';
		i /= 10;
	}
	iter += tempor+1;
	arr[iter] = 0;
	
	return &(arr[0]);
}


void Highscore() {
	clearDisplay();
	char* temp;
	int it = 0;
	short int flag1 = 0;
	short int flag2 = 0;
	short int flag3=0;
	if (howMany >= 2) {
		temp = concat(it++);
		wyswietl(temp,1,0);
		temp = concat(it++);
		wyswietl(temp,2,0);
	} else if (howMany == 1) {
		temp = concat(it++);
		wyswietl(temp,1,0);
	} else {
		wyswietl("Brak wynikow",1,0);
	}
	while (1) {
		if (((P4IN & BIT4) == 0) && (flag1 == 1)) {
			flag1 = 0;
			if (it + 1 > howMany) {
				continue;
			} else {
				clearDisplay();
				temp = concat(it-1);
				wyswietl(temp,1,0);
				temp = concat(it);
				wyswietl(temp,2,0);
				it++;
			}
		}
		if ((P4IN & BIT4) != 0) {
			flag1 = 1;
		}
		if (((P4IN & BIT5) == 0) && (flag2 == 1)) {
			flag2 = 0;
			if (it - 2 <= 0) {
				continue;
			} else {
				clearDisplay();
				temp = concat(it-3);
				wyswietl(temp,1,0);
				temp = concat(it-2);
				wyswietl(temp,2,0);
				it--;
			}
		}
		if ((P4IN & BIT5) != 0) {
			flag2 = 1;
		}
		if ((P4IN & BIT7) != 0) {
			flag3 = 1;
		}
		if (((P4IN & BIT7) == 0)&&(flag3==1)) {
			clearDisplay();
			break;
		}
	}
}



void menu(int LMP3_bits){

	short int flag1 = 0;
 	short int flag2 = 0;
 	short int flag3 = 0;
 	short int flag4 = 0;
  	while(1){

 	wyswietl("Gra",1,0);
	wyswietl("Opis",1,4);
	wyswietl("Autorzy",1,9);
	wyswietl("HighScore",2,7);

	if(((P4IN & BIT4) == 0)&&(flag1 == 1)){
	  	GraEngine(LPM3_bits);
		flag1=0;
		flag2=0;
		flag3=0;
		flag4=0;
	}
  	if(((P4IN & BIT5) == 0)&&(flag2 == 1)){
	  	Opis();
		flag1=0;
		flag2=0;
		flag3=0;
		flag4=0;
	}
	if(((P4IN & BIT6) == 0)&&(flag3 == 1)){
	  	Autorzy();
	  	flag1=0;
		flag2=0;
		flag3=0;
		flag4=0;
	}
	
	if(((P4IN & BIT7) == 0)&&(flag4 == 1)){
		Highscore();
	  	flag1=0;
		flag2=0;
		flag3=0;
		flag4=0;
	}
	 	if((P4IN & BIT4) != 0)flag1=1;
	 	if((P4IN & BIT5) != 0)flag2=1;
	 	if((P4IN & BIT6) != 0)flag3=1;
	 	if((P4IN & BIT7) != 0)flag4=1;
  }
}

void main( void )
{
	P4DIR &= ~BIT4;
	P4DIR &= ~BIT5;
	P4DIR &= ~BIT6;
	P4DIR &= ~BIT7;
	P4DIR |= 0x0C;
	WDTCTL=WDTPW + WDTHOLD;           // Wy31czenie WDT
	InitPortsLcd();                   // inicjalizacja portów LCD
	InitLCD();                        // inicjalizacja LCD
	clearDisplay();                   // czyszczenie wyowietlacza
	
	BCSCTL1 |= XTS;                       // ACLK = LFXT1 = HF XTAL 8MHz
	do
	{
		IFG1 &= ~OFIFG;                     // Czyszczenie flgi OSCFault
		for (i = 0xFF; i > 0; i--);         // odczekanie
	}
	while ((IFG1 & OFIFG) == OFIFG);    // dopóki OSCFault jest ci1gle ustawiona
	
	BCSCTL1 |= DIVA_3;                    // ACLK=8 MHz/8=1 MHz
	BCSCTL2 |= SELM0 | SELM1;             // MCLK= LFTX1 =ACLK
	
	// Timer_A  ustawiamy na 500 kHz
	// a przerwanie generujemy co 100 ms
	TACTL = TASSEL_1 + MC_1 +ID_2;        // Wybieram ACLK, ACLK/4=250kHz,tryb Up
	CCTL0 = CCIE;                         // w31czenie przerwan od CCR0
	CCR0=250000;                           // podzielnik 250: przerwanie co 1 ms
	
	_EINT();                               // w31czenie przerwan
	
	/******************************************************************************/
        
	menu(LPM3_bits);
}


// procedura obs3ugi przerwania od TimerA

#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void) {
	++licznik;
	_BIC_SR_IRQ(LPM3_bits);             // wyjocie z trybu LPM3
}
