//
// Created by g on 2019.05.06..
//

#ifndef UNTITLED2_RSA_H
#define UNTITLED2_RSA_H


#include <string>
#include "UART.h"
using namespace::std;
typedef uint64_t ulli;


class RSA {
private:
    	ulli p;		// N egyik primosztoja
    	ulli q;		// N masik primosztoja
    	ulli publicExp;		// nyilvanos kitevo, ezzel titkosit kuldeskor
    	ulli privateExp;		// titkos kitevo, ezzel fejt ki fogadaskor
    	ulli N;
    	static int primSzamjegyek;		// a hasznalt primszamok szamjegyeit tarolja, itt 4
    	UART& io;		// az az UART objektum, ami a kuldest es a fogadast vegzi



    	ulli const static primGen();		// primSzamjegyek szerinti hosszu primszamot general
    	ulli const static randomSzam();		// primszamjegyek-nek megfelelo hosszu random szamot general
    	bool const static primTeszt(ulli szam);		// eldonti egy szamrol, hogy prim-e
    	ulli const static hatvanyModN(ulli alap, ulli kitevo, ulli modulus);	//gyorshatvanyozassal visszaadja az alap kitevore emelt modulus szerinti maradekat
    	ulli const static lnko(ulli egyik, ulli masik);		// 2 szam legnagyobb kozos osztojat adja vissza
    	ulli const static nedikBit(ulli szam, int bit);		// visszaadja egy uint64_t -ben a szam adott bitjet (LSB=0)
    	int const setup();
public:
    	// konstruktor
    	RSA(UART& ioParam);

    	// kuldes/fogadas
    	void const send(string uzenet);		// az uzenetet a tarolt "io" UART objektumon keresztul elkuldi
    	string const recive();			// hasonloan fogad egy stringet es azt adja vissza

	void resetN(){N=p*q;}			// N-et ujrageneralja, ha uj primeket adtunk meg p-nek es q-nak

    	// getterek/setterek
    	ulli const getN() {return N;}
    	ulli const getfN() {return (p-1)*(q-1);}
    	ulli const getP(){return p;}
    	ulli const getQ(){return q;}
    	ulli const getPublicExp(){return publicExp;}
    	ulli const getPrivateExp(){return privateExp;}
    	int const getPrimSzamjegyek(){return primSzamjegyek;}
    	void setN(ulli ujN){N=ujN;}
    	void setP(ulli ujP){p=ujP;}
    	void setQ(ulli ujQ){q=ujQ;}
    	void setPrivateExp(ulli ujPrivateExp){privateExp=ujPrivateExp;}
    	void setPublicExp(ulli ujPublicExp){publicExp=ujPublicExp;}
    	void setPrimSzamjegyek(int uj){primSzamjegyek=uj;}

};



#endif //UNTITLED2_RSA_H

