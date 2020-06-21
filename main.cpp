#include <iostream>
#include <math.h>
#include "RSA.h"
#include "UART.h"
#include <time.h>
#include <wiringPi.h>
#include <wiringSerial.h>

//#include <bitset>
using namespace::std;



int main()
{
	int maxint = -1;
	maxint = maxint >> 1;
	srand(time(nullptr));
	UART U;
	//RSA titkosito(U);

	char input;
	RSA gagyisag(U);
	RSA menoseg(U);
	while(1){

        cout << "----------------------------------------------------------\n";

        cout << "\nKuldeni(\"k\"), vagy fogadni(\"f\") kivansz?" << endl;
        do{
                cin >> input;
                if(input!='q' && input!='f' && input!='k')
                        cout << "\n\nEgy ertelmes karaktert adj meg!\n\n";
        }while(input!='f' && input!='k' && input!='q');

        if(input=='f')
        {
                cout << "\n\nAz uzenet kuldesehez kell:\nN: " << gagyisag.getN() << "  nyilvanos kitevo: " << gagyisag.getPublicExp();
                cout << "\nUzenet: " << gagyisag.recive();
        }
        else if(input=='k')
        {
                cout << "\nKerem az N-t: ";
                ulli ujN;
                cin >> ujN;
                menoseg.setN(ujN);
                cout << "\nKerem a nyilvanos kitevot: ";
                cin >> ujN;
                menoseg.setPublicExp(ujN);
                cout<< "\nUzenet: ";
                string uzenet;
                cin >> uzenet;
                menoseg.send(uzenet);
        }
        else if(input=='q')
                return 0;

        cout << endl;
	}
	return 0;
}
