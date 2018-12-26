/*
 *	DMA.c
 */

#include "DMA.h"
#include "RCC.h"

int dmaGetStatus(dmaRegs *dma, int streamNum, int flags){

	int Shifts = 0;
	int tempFlags;

	switch(streamNum){
	case 0 : case 4 : Shifts = 0;
					  break;
	case 1 : case 5 : Shifts = 6;
					  break;
	case 2 : case 6 : Shifts = 16;
					  break;
	case 3 : case 7 : Shifts = 22;
					  break;
	}

	tempFlags = flags << Shifts;

	/* eg
	 * 		1 0 0 1		Interrupt status register
	 * 	&	0 0 0 1		tempFlags	(depend on Flag but only one bit is '1')
	 * 	------------
	 * 	    0 0 0 1
	 *
	 */

	if(streamNum <= 3){
		if((dma->LISR & tempFlags))
			return 1;

	}

	else if(streamNum >= 4){
		if((dma->HISR & tempFlags))
			return 1;
	}
	else{
		return 0;
	}

}


void dmaClearStatus(dmaRegs *dma, int streamNum, int flags){
	int Shifts = 0;
	int tempFlags;

	switch(streamNum){
	case 0 : case 4 : Shifts = 0;
					  break;
	case 1 : case 5 : Shifts = 6;
					  break;
	case 2 : case 6 : Shifts = 16;
					  break;
	case 3 : case 7 : Shifts = 22;
					  break;
	}

	tempFlags = ~((flags) << Shifts);

	/* flags =  00000000100
	 *
	 * ~flags = 11111111011
	 * reg    = 01011101100		&
	 * result = 01011101011
	 *
	 *  eg (want to clear bit 3,4)
	 *  	1 1 0 1 	Interrupt status register
	 *  &	0 0 1 1 	tempFlags (Flags = 1 1 0 0)
     *  -----------
     *      0 0 0 1
     **/

	if(streamNum <= 3){
		dma->LISR = (dma->LISR & tempFlags);
	}
	else{
		dma->HISR = (dma->HISR & tempFlags);
	}

}

void dmaStreamConfigure(dmaRegs *dma, int streamNum, int configuration){
	int *temp;
	temp = &((dma->S[streamNum]).CR);
	(dma->S[streamNum]).CR = configuration;
}

// MUST PASS IN MASK PARAMETER IF NEED CHANGE CONFIGURATION
void dmaStreamConfigureOnly(dmaRegs *dma, int streamNum, int mask, int configuration){
	(dma->S[streamNum]).CR &= ~(mask);			// set the desired bits to 0 (based on mask)
	(dma->S[streamNum]).CR |= configuration;
}
