#include<mqx.h>
#include"../dac/dac_prv.h"
void pdb_init(int32_t source, uint32_t samplerate)
{
	/* sampleRates[source] = samplerate; */

	SIM_SCGC6 |= SIM_SCGC6_PDB_MASK ;    // SCGC6 is at address 0x4004_803Ch
	PDB0_SC = PDB_SC_PDBEN_MASK;         	// enable PDB module
	
	   
#if (MQX_CPU == PSP_CPU_MK70F120M)
	PDB0_DACINT0 =60000000/samplerate; 
	
#else
#if MQX_CPU == PSP_CPU_MK60D100M 
	PDB0_DACINT0 =48000000/samplerate;       // Effective after writting PDBSC_DACTOE = 1  (1/48000KHz) * 1088 = 0.0226757369614512ms (44100)
#endif
#endif

	PDB0_MOD = PDB0_DACINT0;

	
	PDB0_SC |= PDB_SC_LDOK_MASK ;      	 // load the value assigned to PDBDACINTV to register
	PDB0_SC |= PDB_SC_TRGSEL(15);        	 // software trigger
	PDB0_SC |= PDB_SC_CONT_MASK ;         	 // CONT mode

	PDB0_DACINTC0 |= PDB_DACINTC0_TOE_MASK ; // DAC output delay from PDB Software trigger
}


void pdb_stop()
{
	PDB0_SC &= ~PDB_SC_PDBEN_MASK; 
}

void pdb_start()
{
	PDB0_SC |= PDB_SC_PDBEN_MASK; 
	PDB0_SC |= PDB_SC_SWTRIG_MASK ;
}
