#ifndef __DMA_H__
#define __DMA_H__

#include <stdint.h>

typedef volatile uint32_t IoRegister;

typedef struct dmaStreamRegs dmaStreamRegs;
struct dmaStreamRegs{
	IoRegister CR;
	IoRegister NDTR;
	IoRegister PAR;
	IoRegister M0AR;
	IoRegister M1AR;
	IoRegister FCR;
};


typedef struct dmaRegs dmaRegs;
struct dmaRegs{
	IoRegister LISR;
	IoRegister HISR;
	IoRegister LIFCR;
	IoRegister HIFCR;
	dmaStreamRegs S[8];
};

#define dma1		((dmaRegs *)0x40026000)
#define dma2		((dmaRegs *)0x40026400)

// Interrupt register
#define DMA_TCIF 	(1 << 5)		// Transfer complete
#define DMA_HTIF 	(1 << 4)		// Half transfer
#define DMA_TEIF 	(1 << 3)		// Transfer error
#define DMA_DMEIF 	(1 << 2)		// DMA error
#define DMA_FEIF 	(1 << 0)		// FIFO error

// DMA stream numbers
#define DMA_STREAM0		0
#define DMA_STREAM1		1
#define DMA_STREAM2		2
#define DMA_STREAM3		3
#define DMA_STREAM4		4
#define DMA_STREAM5		5
#define DMA_STREAM6		6
#define DMA_STREAM7		7

// Configuration register (0 is the default configuration)
// Enable/ Disable DMA stream
#define DMA_STREAM_ED_MASK	(1 << 0)		// Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_DIS 		 (0 << 0)		    // Disable Stream
#define DMA_EN 	    	 (1 << 0)			// Enable Stream

// DMA stream direct mode error interrupt
#define DMA_DMEIE_MASK	(1 << 1)		// Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_DMEIE		(1 << 1)		// Enable Direct mode error interrupt
#define DMA_DMEID		(0 << 1)		// Disable Direct mode error interrupt

// DMA stream transfer error interrupt
#define DMA_TEI_MASK	(1 << 2)   	    // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_TEIE    	(1 << 2)		// Enable Transfer error interrupt
#define DMA_TEID    	(0 << 2)		// Disable Transfer error interrupt

// DMA stream half transfer interrupt
#define DMA_HHT_MASK	(1 << 3)   	    // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_HTIE    	(1 << 3)        // Enable Half transfer interrupt
#define DMA_HTID   		(0 << 3)        // Disable Half transfer interrupt

// DMA stream complete interrupt
#define DMA_TCI_MASK	(1 << 4)   	    // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_TCIE    	(1 << 4)		// Enable Complete interrupt
#define DMA_TCID    	(0 << 4)		// Disable Complete interrupt

// DMA stream flow controller
#define DMA_PFCTRL_MASK	(1 << 5)   	    // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_PFCTRLP 	(1 << 5)		// Set Peripheral to flow controller
#define DMA_PFCTRLD  	(0 << 5)		// Set DMA to flow controller

// DMA stream data transfer direction
#define DMA_DIR_MASK	(3 << 6)   	    // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_DIR_PtM 	(0 << 6)		// Set data transfer direction to Peripheral to Memory
#define DMA_DIR_MtP 	(1 << 6)		// Set data transfer direction to Memory to Peripheral
#define DMA_DIR_MtM 	(2 << 6)		// Set data transfer direction to Memory to Memory
#define DMA_DIR_REV 	(3 << 6)		// Reserved

// DMA stream circular mode
#define DMA_CIRC_MASK	(1 << 8)   	    // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_CIRC		(1 << 8)		// Enable Circular mode
#define DMA_CIRCD		(0 << 8)		// Disable Circular mode

// DMA stream peripheral pointer increment
#define DMA_PINC_MASK	(1 << 9)   	// Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_PINCD		(0 << 9)		// Disable Peripheral pointer increment after each data transfer
#define DMA_PINC		(1 << 9)		// Enable Peripheral pointer increment after each data transfer

// DMA stream memory address pointer increment
#define DMA_MIC_MASK	(1 << 10)   	// Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_MINC		(1 << 10)		// Enable Memory address pointer increment after each data transfer
#define DMA_MINCD		(0 << 10)		// Disable Memory address pointer increment after each data transfer

// DMA stream peripheral data size
#define DMA_PSIZE_MASK	(3 << 11)   	// Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_PSIZE8   	(0 << 11)		// Set peripheral data size to 8-bit
#define DMA_PSIZE16 	(1 << 11)		// Set peripheral data size to 16-bit
#define DMA_PSIZE32 	(2 << 11)		// Set peripheral data size to 32-bit
#define DMA_PSIZER   	(3 << 11)		// Reserved

// DMA memory peripheral data size
#define DMA_MSIZE_MASK	(3 << 13)   	// Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_MSIZE8   	(0 << 13)		// Set memory data size to 8-bit
#define DMA_MSIZE16  	(1 << 13)		// Set memory data size to 16-bit
#define DMA_MSIZE32  	(2 << 13)		// Set memory data size to 32-bit
#define DMA_MSIZER   	(3 << 13)		// Reserved

// DMA stream peripheral increment offset size
#define DMA_PINCOS_MASK	(1 << 15)       // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_PINCOS   	(0 << 15)		// Peripheral increment offset size is PSIZE
#define DMA_PINCOSF  	(1 << 15)		// Peripheral increment offset size is 4 bytes (fixed)

// DMA stream priority
#define DMA_PL_MASK	 (3 << 16)     // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_PLL		 (0 << 16)     // Set the priority level to low
#define DMA_PLM		 (1 << 16)     // Set the priority level to medium
#define DMA_PLH		 (2 << 16)     // Set the priority level to high
#define DMA_PLVH	 (3 << 16)     // Set the priority level to very high

// DMA double buffering
#define DMA_DBM_MASK	(3 << 21)   // Mask to set respective bits into 0 (in dmaConfigureStreamOnly ~MASk)
#define DMA_DBMD	 (0 << 18)		// Disable Double buffer mode
#define DMA_DBME	 (1 << 18)		// Enable Double buffer mode

// DMA memory buffer current target (M0 and M1)
#define DMA_CT_MASK	 (1 << 19)   // Mask to set respective bits into 0
#define DMA_CT0		 (0 << 19)		// Current target Memory 0
#define DMA_CT1		 (1 << 19)		// Current target Memory 1

// DMA peripheral transfer burst rate
#define DMA_PBURST_MASK	(3 << 21)   // Mask to set respective bits into 0
#define DMA_PBURSTS  (0 << 21)     // Single transfer
#define DMA_PBURST4  (1 << 21)     // Incremental burst of 4 beats
#define DMA_PBURST8  (2 << 21)     // Incremental burst of 8 beats
#define DMA_PBURST16 (3 << 21)     // Incremental burst of 16 beats

// DMA memory transfer burst size
#define DMA_MBURST_MASK	(3 << 23)   // Mask to set respective bits into 0
#define DMA_MBURSTS  (0 << 23)		// Single transfer
#define DMA_MBURST4  (1 << 23)      // Incremental burst of 4 beats
#define DMA_MBURST8  (2 << 23)      // Incremental burst of 8 beats
#define DMA_MBURST16 (3 << 23)      // Incremental burst of 16 beats

// DMA channel number selection for DMA stream
#define DMA_CHMASK	(7 << 25)		// Mask to set respective bits into 0
#define DMA_CH0SEL 	(0 << 25)		// Channel 0 selected
#define DMA_CH1SEL 	(1 << 25)		// Channel 1 selected
#define DMA_CH2SEL 	(2 << 25)		// Channel 2 selected
#define DMA_CH3SEL 	(3 << 25)		// Channel 3 selected
#define DMA_CH4SEL 	(4 << 25)		// Channel 4 selected
#define DMA_CH5SEL 	(5 << 25)		// Channel 5 selected
#define DMA_CH6SEL 	(6 << 25)		// Channel 6 selected
#define DMA_CH7SEL 	(7 << 25)		// Channel 7 selected

// MACROS
// Clear all DMA flags
#define dmaClearAllFlags(dma, stream)     dmaClearStatus(dma, stream, DMA_TCIF  |  \
																DMA_HTIF  |	 \
																DMA_TEIF  |  \
																DMA_DMEIF |	 \
																DMA_FEIF)


// Enable interrupts
#define DMA_INTERRUPT_ENABLE(dma, stream, interrupts)		\
				do{											\
					(dma)->S[stream].CR &= ~(0xf << 1);		\
					(dma)->S[stream].CR |= interrupts;		\
				}while(0)

// Disable interrupts
#define DMA_INTERRUPT_DISABLE(dma, stream, interrupts)		\
					(dma)->S[stream].CR &= ~(interrupts)

/*
// Enable stream
#define DMA_STREAM_ENABLE(dma, stream)		\
		do{											\
			(dma)->S[stream].CR &= ~(DMA_EN);		\
			(dma)->S[stream].CR |= DMA_EN;		\
		}while(0)
*/

// Enable stream
#define DMA_STREAM_ENABLE(dma, stream)				\
					(dma)->S[stream].CR |= DMA_EN


// Disable stream
#define DMA_STREAM_DISABLE(dma, stream)		\
					(dma)->S[stream].CR &= ~(DMA_EN)

// Functions declaration
void dmaStreamConfigure(dmaRegs *dma, int streamNum, int configuration);
void dmaStreamConfigureOnly(dmaRegs *dma, int streamNum, int mask, int configuration);
int  dmaGetStatus(dmaRegs *dma, int streamNum, int flags);
void dmaClearStatus(dmaRegs *dma, int streamNum, int flags);

#endif // __DMA_H__
