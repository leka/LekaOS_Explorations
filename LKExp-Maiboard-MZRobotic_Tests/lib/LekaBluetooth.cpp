#include "LekaBluetooth.h"

LekaBluetooth::LekaBluetooth(PinName pin_tx, PinName pin_rx) : _bm64(pin_tx, pin_rx) {}

void LekaBluetooth::runTest() {
	printf("\nTest of bluetooth!\n");
	//DRV_BM64_Initialize();

	//char BM64_Recu[128];
	DigitalOut BM64_RESET(PC_8,
						  0);	// déclare en sortie et l'initialise au NLB (Niveau Logique Bas)
	wait_us(100000);
	BM64_RESET = 1;

	_bm64.set_baud(115200);
	char buf[32] = {0x08};

	while (1) {
		/*
			 if( PC.readable() > 0){
			   for (int i=0; i<64; i++){
				   char c = PC.getc(); //Charger le caractère reçu dans la variable
				   if (c == 'Z') { break;}
				   PC_Recu[i] = c;   //placer le caractère dans le tableau
				   BM64.printf("%c",c);
			   } //end of for
			 BM64.printf("\n");
			 //BM64.printf(PC_Recu); // Envoie vers le BM64
			 } //end of if (PC.readable() > 0
	   //*/
		//_bm64.sprintf("0x08\n");
		_bm64.write(buf, 1);
		    if (uint32_t num = _bm64.read(buf, sizeof(buf))) {
            // Echo the input back to the terminal.
            _bm64.write(buf, num);
        }

		if (_bm64.readable() > 0) {
			uint32_t num = _bm64.read(buf, sizeof(buf));
			printf("Charactere recu = ");
			
			for (int i = 0; i < num; i++) {
				//char c = _bm64.getc();	 // Charger le caractère reçu dans la variable
				printf("%X", buf[i]);
				//BM64_Recu[i] = c;	// placer le caractère dans le tableau
			}						// end of for

			printf("\n");
			// PC.printf(BM64_Recu); // Envoie vers le BM64
		}	// end of if (PC.readable() > 0
/*
		ThisThread::sleep_for(1000);
		_bm64.printf("AT+NAME?\n");

		if (_bm64.readable() > 0) {
			printf("Charactere recu = ");
			for (int i = 0; i < 128; i++) {
				char c = _bm64.getc();	 // Charger le caractère reçu dans la variable
				printf("%X", c);
				BM64_Recu[i] = c;	// placer le caractère dans le tableau
			}						// end of for
		 	printf("\n");
			// PC.printf(BM64_Recu); // Envoie vers le BM64
		}	// end of if (PC.readable() > 0
*/
		
	}
	printf("\n");
}
