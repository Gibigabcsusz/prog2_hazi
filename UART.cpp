//
// Created by g on 2019.05.01..
//
//char leallitoUzenet[6]="\0\0\x6F\x9D\x49";
//
#include "UART.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>

#include <wiringPi.h>
#include <wiringSerial.h>


const char* UART::port="/dev/ttyS0";
using namespace::std;




int UART::setup()			// A soros kommunikaciot kesziti elo, megprobalja megnyitni a "/dev/ttyS0" serial portot,
{					// majd a wiringPi library hasznalatahoz szukseges wiringPiSetup() fuggvenyt probalja meghivni.
	int fd;				// Az elozoek sikertelensege eseten sorrendben 1, illetve 2 ertekekkel ter vissza

    	if ((fd = serialOpen (port, 115200)) < 0)
    	{
        	fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno));
        	return 1;
    	}

    	if (wiringPiSetup () == -1)
    	{
		fprintf (stderr, "Unable to start wiringPi: %s\n", strerror (errno));
		return 2;
	}
	return fd;
}


UART::UART(): fd(setup()) {}

UART::~UART()
{
	if(fd!=1)
		serialClose(fd);
}


ulli const UART::CS(ulli szam)	// a szam vegere tesz egy byte checksumot, amit az elso 4 byte alapjan general,  es visszater az igy kapott szammal
{
	ulli masolat = szam;
	ulli cs = 0;
	for(int i=0; i<4; i++)		// checksum generalas, a checksum byte a szam byte-jainak a bitenkent vett XOR-ja
	{
		cs ^= masolat & 0xFF;
		masolat =masolat >> 8;
	}
	return (szam << 8) + cs;
}





bool const UART::checkCS(ulli szam)		// az 5 byte-os szam nagyobb helyierteku 4 byte-jahoz legeneralja a checksumot,
{						// true-val ter vissza, ha ez megegyezik az 5. byte-tal, false-szal, ha nem
        ulli eredeti = szam >> 8;
        if(CS(eredeti) == szam)
                return true;
        return false;
}


void const UART::serialSend(ulli packet)	// A kapott 4 byte-on abrazolhato szamhoz legeneralja a checksumot, az igy kapott 5 byte-os szamot szetdarabolja byte-okra,
{						// majd a legnagyobb helyierteken levo byte-tal kezdve elkuldi a soros porton keresztul ezeket.
	int csUtaniHossz=5;			// Ha elkuldte az 5 byteot, 1 byte-nyi visszajelzest var. Ha az erkezo byte 'g', akkor visszater.
        char biteok[csUtaniHossz];		// Ha az erkezo byte barmi mas, vagy 10s-on keresztul nem erkezett byte, akkor ujrakuldi az 5 byte-ot.

	packet = CS(packet);

	for(int i=0; i < csUtaniHossz; i++)
        {
                biteok[i] = (char)((packet >> 8*(csUtaniHossz-1-i)) & 0xFF);
        }

	char feedback;

	do{
        	for(int i=0; i < csUtaniHossz; i++)
        	{
                	serialPutchar(fd, biteok[i]);
			cout << (int)biteok[i] << " - ";
        		cout.flush();
		}
		feedback = serialGetchar(fd);
	}while(feedback!='g');
}

ulli const UART::serialRecive()			// Soros porton var 5 bytenyi erkezo adatot. Ha jott 5 bytenyi, az elso 4 byte-hoz legeneralja a checksumot,
{						// ezt osszeveti az 5. byte-tal. Ha megegyezik, visszakuld a soros porton egy 'g' karaktert, es visszater
	int csUtaniHossz=5;			// az elso 4 erkezett karakterbol letrehozott szammal,
	ulli erkezett;				// (az elsonek erkezo karakter kerul a legnagyobb helyierteku byte-ra).
	do{					// Ha a checksum azonban nem egyezik, vagy 10s-on keresztul nem erkezett semmi, a soros porton egy
		int i = 0;			// 'b' karakter kuldese utan ujra 5 byte-ot var a soros porton, elolrol kezdve a muveletet.
		erkezett = 0;
		while (i < csUtaniHossz)
    		{
        		if(serialDataAvail(fd))
        		{
				char bite = serialGetchar(fd);
            			//cout << "\nprobalkozas: " << (int)bite;
				//cout.flush();
				erkezett += ((ulli)bite) << 8*(csUtaniHossz-1-i);
            			i++;
        		}
		}
		if(checkCS(erkezett))
			serialPutchar(fd, 'g');
		else
			serialPutchar(fd, 'b');
		//cout << "\n\ncrc:" << checkCS(erkezett);
		//cout.flush();
	}while(!checkCS(erkezett));
	return erkezett >> 8;
}
