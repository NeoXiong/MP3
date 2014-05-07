backup change for mfs.a and psp.a
K70 dir --- replace these libs to sources\libs for K70 project 
K60 dir --- replace these libs to sources\libs for K60 project


k60 project: 
///////////////////////////////////////
intflash.lcf:
main stack size from 1024 to 2048
__DEFAULT_INTERRUPT_STACK_SIZE = 2048;
///////////////////////////////////////

///////////////////////////////////////
// build hmi to bsp.a
///////////////////////////////////////
or add to mqx_patch :
bsp_twrk60d100m.bat add below:
copy %MQXROOTDIR%\mqx\source\io\hmi\btnled.h %OUTPUTDIR%\btnled.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\derivative.h %OUTPUTDIR%\derivative.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\hmi_client.h %OUTPUTDIR%\hmi_client.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\hmi_lwgpio_provider.h %OUTPUTDIR%\hmi_lwgpio_provider.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\hmi_provider.h %OUTPUTDIR%\hmi_provider.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\hmi_tss_provider.h %OUTPUTDIR%\hmi_tss_provider.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\hmi_uids.h %OUTPUTDIR%\hmi_uids.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_API.h %OUTPUTDIR%\tss\TSS_API.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_DataTypes.h %OUTPUTDIR%\tss\TSS_DataTypes.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_GPIO.h %OUTPUTDIR%\tss\TSS_GPIO.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_Sensor.h %OUTPUTDIR%\tss\TSS_Sensor.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_SensorGPIO.h %OUTPUTDIR%\tss\TSS_SensorGPIO.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_SensorTSI.h %OUTPUTDIR%\tss\TSS_SensorTSI.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_SensorTSIL.h %OUTPUTDIR%\tss\TSS_SensorTSIL.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_StatusCodes.h %OUTPUTDIR%\tss\TSS_StatusCodes.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_SystemSetupVal.h %OUTPUTDIR%\tss\TSS_SystemSetupVal.h /Y
copy %MQXROOTDIR%\mqx\source\io\hmi\TSS\shared\TSS_Timer.h %OUTPUTDIR%\tss\TSS_Timer.h /Y
 ??? TSS_SystemSetup.h 

bsp.h add below:
/* HMI */
#include <hmi_uids.h>
#include <btnled.h>
#include <hmi_client.h>
#include <hmi_lwgpio_provider.h>
#include <hmi_tss_provider.h>
#include <hmi_provider.h>

HMI_CLIENT_STRUCT_PTR _bsp_btnled_init(void);
_mqx_int _bsp_tss_io_init(void);
_mqx_int _bsp_btnled_set_twrpi(_mqx_int);
_mqx_int _bsp_btnled_get_twrpi(void);




twrk60d100m.h add below:
// for hmi
#define BSP_TSI_INT_LEVEL       (4)

/* HMI Touch TWRPI daughter cards */
#define BSP_TWRPI_VOID   0
#define BSP_TWRPI_ROTARY 1
#define BSP_TWRPI_KEYPAD 2


