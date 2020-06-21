//
// Created by g on 2019.05.06..
//

#include "RSA.h"
#include "UART.h"
#include <random>
#include <time.h>
//#include <math.h>
#include <iostream>
#include <wiringPi.h>
#include <wiringSerial.h>

using namespace::std;

int RSA::primSzamjegyek=4;

ulli const RSA::randomSzam()
{
    ulli szam=0;
    while(szam==0)                          // az első ciklus azt biztosítja, hogy ne legyen kevesebb, mint 4 jegyű a szám
    szam=rand()%10;                         // itt a szám egyjegyű, nem nulla lesz
    for(int i=1; i<primSzamjegyek; i++)     // a második cikllus a maradék 3 számjegyet generálja
    {
        szam=szam*10+rand()%10;
    }
    return szam;
}


bool const RSA::primTeszt(ulli szam)
{
    if(szam %2 == 0 && szam!=2)             	// 2, es a tobbi 2-vel oszthato szam kezelese
        return false;
    if(szam==2) return true;
    if(szam<2) return false;                	// 0 es 1 kezelese
    for(ulli i=3; i*i<=szam; i+=2)          	// 3-mal kezdve a vizsgalando szam gyokeig minden szammal veszi a vizsgalando szam maradekat
        if(szam%i==0)
            return false;
    return true;				// ha egyik gyokenel kisebb szammal sem oszthato (es a gyokevel sem), akkor prim
}

ulli const RSA::primGen()                   // addig general random 4-jegyu szamokat, amig nem talal primet
{
    ulli aktual=0;
    do{
        aktual=randomSzam();
    }while(!primTeszt(aktual));
    return aktual;
}

ulli const RSA::nedikBit(ulli szam, int n)	// visszater a "szam" "n"-edik bitjevel
{
    ulli maszk=1;
    if((szam & (maszk << n)))
        return 1;
    return 0;
}

ulli const RSA::hatvanyModN(ulli alap, ulli kitevo, ulli modulus)		// gyorshatvanyozas
{
    ulli aktualHatvany=alap;
    ulli valasz=1;
    for(int i=0; i<64; i++)
    {

        if(nedikBit(kitevo, i)==1)
        {
            valasz=(valasz*aktualHatvany)%modulus;
        }
        aktualHatvany=(aktualHatvany*aktualHatvany)%modulus;
    }
    return valasz;
}

ulli const RSA::lnko(ulli egyik, ulli masik)        //euklideszi algoritmus
{
    while(masik!=0) {
        if (egyik < masik) {                        // egyik > masik legyen
            ulli temp = masik;
            masik = egyik;
            egyik = temp;
        }
        egyik=egyik-masik;
    }
    return egyik;
}


RSA::RSA(UART& ioParam): io(ioParam)
{
    int k;	// szamolja, hanyszor probalt mar az adott primekkel titkos kulcsot generalni,
		// 10000 proba utan masik primeket general, es azokkal probalkozik tovabb
    do{
        p=primGen();
        q=primGen();
        N=p*q;
        //nyilvanos kulcs generalasa, 1<publicExp<fN �es lnko(publicExp,fN)=1
        ulli fN=getfN();
        for (publicExp = 3; lnko(publicExp, fN) != 1; publicExp++);

        //titkos kulcs generalasa

        k=2;
        do{
            privateExp=(1+k*fN)/publicExp;
            k++;
        }while((publicExp*privateExp)%fN!=1 && k<10000);

    }while(k>=10000 || N<17000000);
}

//////////////////////////////////////////////////////////////////////////
//									//	MEGJEGYZES: Az RSA objektumok a "titkositas"-t a
//	soros porton valo kuldest es fogadast vegzo fuggvenyek		//	"publicExp"-hasznalataval vegzik, a "visszafejtest" pedig
//									//	a "privateExp"-pel.
//////////////////////////////////////////////////////////////////////////

void const RSA::send(string uzenet)		// A kapott "uzenet" stringet 3-mal oszthato hosszura kiegesziti 'space' karakterekkel.
{						// A megfelelo hosszu uzenetet ezutan 3 byte-nyi darabokra bontja. Egy ilyen darab 3 byte-jat
    	int packetek = uzenet.length()/3;	// egy int 3 legkisebb byte-jaba irja (az elso karkter kerul a legnagyobb helyierteku byte-ra).
    	if(uzenet.length() % 3 != 0)		// Az igy kapott 3 byte-os szamra meghivja az RSA titkosito fuggvenyt, egy 4 byte-on
        packetek++;				// abrazolhato szamot kapva. Erre a 4 byte-on elfero szamra hivja meg az "io" UART tagvaltozo
						// serialSend() fuggvenyet.
    	uzenet.resize(packetek*3, ' ');				//uzenet kiegeszitese 3-mal oszthato szamu karakterre

    	ulli intPacket;
    	//cout << "N: " << N << "  Privat kitevo: " << privateExp << endl;
    	for(int j = 0; j < packetek; j++)
    	{
        	intPacket=0;
        	for(int i=0; i<3; i++)      				// int64_t packet eloallitasa a stringbol
        	{
        		intPacket += (ulli)(char)(uzenet[3*j+i]) << (2-i)*8;
        	}

        	io.serialSend(hatvanyModN(intPacket, publicExp, N));	//titkositas es kuldes "io" UART objektummal
    	}
	io.serialSend(111);			// A teljes uzenet elkuldese utan egy 111 int-erteku, titkositatlan packetet kuld,
}						// ezzel jelezve az uzenet veget a fogadonak.



string const RSA::recive()		// Az "io" UART tagvaltozo serialRecive() fuggvenyevel a soros portrol beolvas egy 4 byteos szamot, ez a packet.
{					// A packet-re meghivja a visszafejto RSA fuggvenyt, igy kapva 3 byte-nyi adatot.
	string uzenet;			// Az igy kapott 3 karaktert az "uzenet" stringhez adja. A 111 int-erteku packet jelzi az uzenet veget.
	string stringPacket;		// Ha megjott a lezaro packet, az eddig osszegyujtott uzenet stringgel visszater.
	ulli eredetiDzsumbuj;
	do{
		ulli dzsumbuj = io.serialRecive();
		eredetiDzsumbuj = dzsumbuj;

		stringPacket="";
    		dzsumbuj = hatvanyModN(dzsumbuj, privateExp, N);	// visszafejtes
    		for(int i=0; i<3; i++)
    		{
			stringPacket+=(char)(dzsumbuj >> (2-i)*8 & 255);
		}
		if(eredetiDzsumbuj != 111)		// a 111 eteku 3-bajtos packet az "uzenet vege" jelzes
			uzenet+=stringPacket;
//			cout << eredetiDzsumbuj << endl;
//			cout.flush();
	}while(eredetiDzsumbuj != 111);
	return uzenet;
}

