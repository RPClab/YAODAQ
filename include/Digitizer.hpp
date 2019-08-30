#ifndef DIGITIZER_H
#define DIGITIZER_H
#include <vector>
#include <string>
#include "Data.hpp"
#include <iostream>
#include "WDplot.hpp"
#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>
#include "WDplot.hpp"
#include "WDconfig.hpp"
#include "fft.hpp"
#include "X742CorrectionRoutines.hpp"
#include <thread>
#include <chrono>

#define PLOT_WAVEFORMS   0
#define PLOT_FFT         1
#define PLOT_HISTOGRAM   2



/* Error messages */
typedef enum  {
    ERR_NONE= 0,
    ERR_CONF_FILE_NOT_FOUND,
    ERR_DGZ_OPEN,
    ERR_BOARD_INFO_READ,
    ERR_INVALID_BOARD_TYPE,
    ERR_DGZ_PROGRAM,
    ERR_MALLOC,
    ERR_INTERRUPT,
    ERR_READOUT,
    ERR_EVENT_BUILD,
    ERR_HISTO_MALLOC,
    ERR_UNHANDLED_BOARD,
    ERR_OUTFILE_WRITE,
	ERR_OVERTEMP,

    ERR_DUMMY_LAST,
} ERROR_CODES;

class Digitizer
{
public:
 void EnableChannelForPloting(const int& ch);
 void NextGroup();
 void Quit()
 {
	dat.WDrun.Quit = 1;
 }

void Interrupt();



 void SoftwareTrigger()
 {
	CAEN_DGTZ_SendSWtrigger(handle);
 }
 void Calibrate_XX740_DC_Offset();
 void Set_relative_Threshold();
 void Calibrate_DC_Offset();
 ~Digitizer();
 void Connect();
 int ProgramDigitizer();
 void GetInfos();
 void GetMoreBoardInfo();
 void InterruptTimeout();
 void Quit(const int& error);
 int Set_calibrated_DCO(const int& ch);
 Digitizer(Data& da):dat(da){;}
 int WriteOutputFiles(void *Event);
 int WriteOutputFilesx742(CAEN_DGTZ_X742_EVENT_t *Event);
 void calibrate();



void ProgramDigitizer2()
{


    if (ProgramDigitizer()) {
        Quit(ERR_DGZ_PROGRAM);
    }

}




void ReloadConf()
{
 int ret{0};
if(ReloadCfgStatus > 0) {
        ret = CAEN_DGTZ_GetInfo(handle, &dat.BoardInfo);
        if (ret) {
            Quit(ERR_BOARD_INFO_READ);
        }
        GetMoreBoardInfo();



}
}


void Read()
{
int ret{0};
ret = CAEN_DGTZ_ReadData(handle, CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, dat.buffer, &BufferSize);
        if (ret) {
            Quit(ERR_READOUT);
        }
        NumEvents = 0;
        if (BufferSize != 0) {
            ret = CAEN_DGTZ_GetNumEvents(handle, dat.buffer, BufferSize, &NumEvents);
            if (ret) {
                Quit(ERR_READOUT);
            }
        }
		else {
			uint32_t lstatus;
			ret = CAEN_DGTZ_ReadRegister(handle, CAEN_DGTZ_ACQ_STATUS_ADD, &lstatus);
			if (ret) {
				printf("Warning: Failure reading reg:%x (%d)\n", CAEN_DGTZ_ACQ_STATUS_ADD, ret);
			}
			else {
				if (lstatus & (0x1 << 19)) {
					Quit( ERR_OVERTEMP);
				}
			}
		}




}




void Ugly()
{
int ret{0};
  // Select the next enabled group for plotting
    if ((dat.WDcfg.EnableMask) && (dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE || dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE))
        if( ((dat.WDcfg.EnableMask>>dat.WDrun.GroupPlotIndex)&0x1)==0 )
            GoToNextEnabledGroup();

    // Read again the board infos, just in case some of them were changed by the programming
    // (like, for example, the TSample and the number of channels if DES mode is changed)
    ReloadConf();

        // Reload Correction Tables if changed
        if(dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX742_FAMILY_CODE && (ReloadCfgStatus & (0x1 << CFGRELOAD_CORRTABLES_BIT)) ) {
            if(dat.WDcfg.useCorrections != -1) { // Use Manual Corrections
                uint32_t GroupMask = 0;

                // Disable Automatic Corrections
                if ((ret = CAEN_DGTZ_DisableDRS4Correction(handle)) != CAEN_DGTZ_Success)
                    Quit(ERR_INVALID_BOARD_TYPE);

                // Load the Correction Tables from the Digitizer flash
                if ((ret = CAEN_DGTZ_GetCorrectionTables(handle, dat.WDcfg.DRS4Frequency, (void*)X742Tables)) != CAEN_DGTZ_Success)
                    Quit(ERR_INVALID_BOARD_TYPE);

                if(dat.WDcfg.UseManualTables != -1) { // The user wants to use some custom tables
                    uint32_t gr;
                    GroupMask = dat.WDcfg.UseManualTables;

                    for(gr = 0; gr < dat.WDcfg.MaxGroupNumber; gr++) {
                        if (((GroupMask>>gr)&0x1) == 0)
                            continue;
                        LoadCorrectionTable(dat.WDcfg.TablesFilenames[gr], &(X742Tables[gr]));
                    }
                }
                // Save to file the Tables read from flash
                GroupMask = (~GroupMask) & ((0x1<<dat.WDcfg.MaxGroupNumber)-1);
                SaveCorrectionTables("X742Table", GroupMask, X742Tables);
            }
            else { // Use Automatic Corrections
                if ((ret = CAEN_DGTZ_LoadDRS4CorrectionData(handle, dat.WDcfg.DRS4Frequency)) != CAEN_DGTZ_Success)
                    Quit(ERR_INVALID_BOARD_TYPE);
                if ((ret = CAEN_DGTZ_EnableDRS4Correction(handle)) != CAEN_DGTZ_Success)
                    Quit(ERR_INVALID_BOARD_TYPE);
            }
        }
    }















void Allocate()
{
 int ret{0};
if(dat.WDcfg.Nbit == 8)
        ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&dat.Event8);
    else {
        if (dat.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE) {
            ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&dat.Event16);
        }
        else {
            ret = CAEN_DGTZ_AllocateEvent(handle, (void**)&dat.Event742);
        }
    }
    if (ret != CAEN_DGTZ_Success) {
        Quit(ERR_MALLOC);
    }
    ret = CAEN_DGTZ_MallocReadoutBuffer(handle, &dat.buffer,&AllocatedSize); /* WARNING: This malloc must be done after the digitizer programming */
    if (ret) {
        Quit(ERR_MALLOC);
    }


}


void LoadDACCalibration()
{

if (dat.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)//XX742 not considered
		::Load_DAC_Calibration_From_Flash(handle, &dat.WDcfg, dat.BoardInfo);

}


void PerformCalibration()
{
  if (dat.WDcfg.StartupCalibration)
        calibrate();
}

void MaskChannels()
{

    if ((dat.BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE) && (dat.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)){
        dat.WDcfg.EnableMask &= (1<<dat.WDcfg.Nch)-1;
    } else {
        dat.WDcfg.EnableMask &= (1<<(dat.WDcfg.Nch/8))-1;
    }
    if ((dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX751_FAMILY_CODE) && dat.WDcfg.DesMode) {
        dat.WDcfg.EnableMask &= 0xAA;
    }
    if ((dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX731_FAMILY_CODE) && dat.WDcfg.DesMode) {
        dat.WDcfg.EnableMask &= 0x55;
    }


}


void SetPlotMask()
{
      if ((dat.BoardInfo.FamilyCode != CAEN_DGTZ_XX740_FAMILY_CODE) && (dat.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)){
        dat.WDrun.ChannelPlotMask = dat.WDcfg.EnableMask;
    } else {
        dat.WDrun.ChannelPlotMask = (dat.WDcfg.FastTriggerEnabled == 0) ? 0xFF: 0x1FF;
    }
	if ((dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE) || (dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE)) {
		dat.WDrun.GroupPlotSwitch = 0;
	}

}










void GoToNextEnabledGroup();
int32_t BoardSupportsCalibration() ;
int32_t BoardSupportsTemperatureRead() ;
 void setPrevRateTime()
 {
	PrevRateTime = get_time();
 }
 void Trigger()
 {

				if (!dat.WDrun.ContinuousTrigger) 
				{
        	CAEN_DGTZ_SendSWtrigger(handle);
          std::cout<<"Single Software Trigger issued"<<std::endl;
          
        }

 } 


  void ContinuousTrigger()
  {
		dat.WDrun.ContinuousTrigger ^= 1;
			if (dat.WDrun.ContinuousTrigger) std::cout<<"Continuous trigger is enabled"<<std::endl;
      else std::cout<<"Continuous trigger is disabled"<<std::endl;

  }

void ContinuousPloting()
{

if (dat.WDrun.ChannelPlotMask == 0) std::cout<<"No channel enabled for plotting"<<std::endl;
      else dat.WDrun.ContinuousPlot ^= 1;
}

 void Plot()
{
 if (dat.WDrun.ChannelPlotMask == 0) std::cout<<"No channel enabled for plotting"<<std::endl;
       else dat.WDrun.SinglePlot = 1;

}

void f()
{

	dat.WDrun.PlotType = (dat.WDrun.PlotType == PLOT_FFT) ? PLOT_WAVEFORMS : PLOT_FFT;
        dat.WDrun.SetPlotOptions = 1;
}


void h()
{
dat.WDrun.PlotType = (dat.WDrun.PlotType == PLOT_HISTOGRAM) ? PLOT_WAVEFORMS : PLOT_HISTOGRAM;
      dat.WDrun.RunHisto = (dat.WDrun.PlotType == PLOT_HISTOGRAM);
      dat.WDrun.SetPlotOptions = 1;

}


void Write()
{
if (!dat.WDrun.ContinuousWrite) dat.WDrun.SingleWrite = 1;

}


void ContinuousWrite()
{
  dat.WDrun.ContinuousWrite ^= 1;
        if (dat.WDrun.ContinuousWrite) std::cout<<"Continuous writing is enabled"<<std::endl;
        else std::cout<<"Continuous writing is disabled"<<std::endl;
}

void Start()
{

if (dat.WDrun.AcqRun == 0) 
			{
				if (dat.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)/*XX742 not considered*/ Set_relative_Threshold();
				if (dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX730_FAMILY_CODE || dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX725_FAMILY_CODE) dat.WDrun.GroupPlotSwitch = 0;
				std::cout<<"Acquisition started"<<std::endl;
       	CAEN_DGTZ_SWStartAcquisition(handle);
				dat.WDrun.AcqRun = 1;
			}
	

}


void Stop()
{
	if (dat.WDrun.AcqRun == 1)
			{
				 std::cout<<"Acquisition stopped"<<std::endl;
         CAEN_DGTZ_SWStopAcquisition(handle);
         dat.WDrun.AcqRun = 0;
			}

}

void Temperature()
{

if (BoardSupportsTemperatureRead()) 
			{
      	if (dat.WDrun.AcqRun == 0) 
				{
        	int32_t ch;
          for (ch = 0; ch < (int32_t)dat.BoardInfo.Channels; ch++) 
					{
          	uint32_t temp;
            int32_t ret = CAEN_DGTZ_ReadTemperature(handle, ch, &temp);
            std::cout<<"CH"<<ch<<": ";
            if (ret == CAEN_DGTZ_Success) std::cout<<temp<<" C"<<std::endl;
						else std::cout<<"CAENDigitizer ERR"<<ret<<std::endl;
          }
					std::cout<<std::endl;
        }
        else std::cout<<"Can't run temperature monitor while acquisition is running."<<std::endl;
			}
			else std::cout<<"Board Family doesn't support ADC Temperature Monitor."<<std::endl;

}

void D()
{

	if (dat.WDrun.AcqRun == 0) 
				{
					std::cout<<"Disconnect input signal from all channels and press any key to start."<<std::endl;
					///FIX ME
					if (dat.BoardInfo.FamilyCode == CAEN_DGTZ_XX740_FAMILY_CODE)/*XX740 specific*/Calibrate_XX740_DC_Offset();
					else if (dat.BoardInfo.FamilyCode != CAEN_DGTZ_XX742_FAMILY_CODE)/*XX742 not considered*/ Calibrate_DC_Offset();
					//set new dco values using calibration data
					for (std::size_t i = 0; i < dat.BoardInfo.Channels; i++) 
					{
						if (dat.WDcfg.EnableMask & (1 << i)) 
						{
							if(dat.WDcfg.Version_used[i] == 1)	Set_calibrated_DCO(i);
							else 
							{
								CAEN_DGTZ_ErrorCode err = CAEN_DGTZ_SetChannelDCOffset(handle, (uint32_t)i, dat.WDcfg.DCoffset[i]);
								if (err) std::cout<<"Error setting channel "<<i<<" offset"<<std::endl;
							}
						}
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					std::cout<<"DAC calibration ready!!"<<std::endl;
				}
				else std::cout<<"Acquisition is running. Stop acquisition to start DAC calibration."<<std::endl;


}


private:
 bool isVMEDevice()
 {	return dat.WDcfg.BaseAddress ? 1 : 0;
 }
 uint32_t AllocatedSize{0};
 uint32_t BufferSize{0};
 uint32_t NumEvents{0};
 uint64_t PrevRateTime{0};
 const static CAEN_DGTZ_IRQMode_t INTERRUPT_MODE{CAEN_DGTZ_IRQ_MODE_ROAK};
 int WriteRegisterBitmask(uint32_t address, uint32_t data, uint32_t mask);
 CAEN_DGTZ_DRS4Correction_t X742Tables[MAX_X742_GROUP_SIZE];
 static long get_time();
 Digitizer()=delete;
 int  handle{-1};
 int ReloadCfgStatus{0x7FFFFFFF}; // Init to the bigger positive number
 Data& dat;
static std::vector<std::string> ErrMsg;
};
#endif