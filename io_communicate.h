#ifndef IO_COMMUNICATE_H
#define IO_COMMUNICATE_H

#include "cbw.h"
#include <iostream>

#define MAXNUMDEVS 100

int EXTCCONV cbDeclareRevision(float *RevNum);
int EXTCCONV cbErrHandling(int ErrReporting, int ErrHandling);
int EXTCCONV cbVOut(int BoardNum, int Chan, int Gain, float DataValue, int Options);
int EXTCCONV cbGetDaqDeviceInventory(DaqDeviceInterface InterfaceType, DaqDeviceDescriptor *Inventory, INT *NumberOfDevices);
int EXTCCONV cbReleaseDaqDevice(int BdNum);

extern double pid_voltage_offset;
extern double opc_voltage_offset;

class communicate_with_io {
public:
  communicate_with_io();
  ~communicate_with_io();
  int set_voltage(float DataValue, int Channel);
  int stop();
  void pid_update_voltages(std::pair < double, double > nextVoltage);
  void one_pole_update_voltages (std::pair < int, double > nextVoltage);

private:
  float RevLevel;
  int numberOfDevices;
  int ULStat;
  DaqDeviceDescriptor inventory[MAXNUMDEVS];
  DaqDeviceDescriptor DeviceDescriptor;
};

#endif // IO_COMMUNICATE_H
