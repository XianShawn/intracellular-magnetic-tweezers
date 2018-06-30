#include "io_communicate.h"

communicate_with_io::communicate_with_io() {
  RevLevel = (float)CURRENTREVNUM;

  /* Declare UL Revision Level */
  ULStat = cbDeclareRevision(&RevLevel);	// Must be the first UL Library function call
  /* Initiate error handling
      Parameters:
          PRINTALL :all warnings and errors encountered will be printed
          DONTSTOP :program will continue even if error occurs.
                   Note that STOPALL and STOPFATAL are only effective in
                   Windows applications, not Console applications.
 */
  cbErrHandling (PRINTALL, DONTSTOP);
}

communicate_with_io::~communicate_with_io() {
  stop();
  for (int BoardNum = 0; BoardNum < numberOfDevices; BoardNum++) {
      ULStat = cbReleaseDaqDevice(BoardNum);
    }
}

int communicate_with_io::set_voltage(float DataValue, int Channel) { // DataValue is what you want to send in as a signal
  int BoardNum = 0;
  int Gain = BIP10VOLTS;
  int Options = DEFAULTOPTION;

  ULStat = cbVOut(BoardNum, Channel, Gain, DataValue, Options);
//  ULStat = 0;
  if(ULStat == NOERRORS) {
    switch (Channel) {
      case 0:
	std::cout << DataValue << " volts to left pole." << std::endl;
	break;
      case 1:
	std::cout << DataValue << " volts to right pole." << std::endl;
	break;
      case 2:
	std::cout << DataValue << " volts to up pole." << std::endl;
	break;
      case 3:
	std::cout << DataValue << " volts to down pole." << std::endl;
	break;
      case 4:
	std::cout << DataValue << " volts pulling out of dish." << std::endl;
	break;
      case 5:
	std::cout << DataValue << " volts pulling into dish." << std::endl;
	break;
    }
    return 0;
  }
  return 1;
}

int communicate_with_io::stop() {
  if (set_voltage (0, 0)) return 1;
  if (set_voltage (0, 1)) return 1;
  if (set_voltage (0, 2)) return 1;
  if (set_voltage (0, 3)) return 1;
  if (set_voltage (0, 4)) return 1;
  if (set_voltage (0, 5)) return 1;
  return 0;
}

// Left == 0
// Right == 1
// Up == 2
// Down == 3
// Off the dish == 4
// Into the dish == 5

// x > 0 if it's to the left of the target
// y > 0 when it's above the target
void communicate_with_io::pid_update_voltages(std::pair <double, double > nextVoltage) {
//  if (nextVoltage.first > 0) { // We need to go right
//      std::cout << "Left: " << pid_voltage_offset - nextVoltage.first << ", right: " << pid_voltage_offset + nextVoltage.first << std::endl;
//      if (set_voltage(pid_voltage_offset + nextVoltage.first, 1)) return; // +ve voltage to the right
//      if (set_voltage(pid_voltage_offset - nextVoltage.first, 0)) return; // -ve voltage to the left
//  } else { // We need to go left
//      std::cout << "Left: " << pid_voltage_offset + nextVoltage.first << ", right: " << pid_voltage_offset - nextVoltage.first << std::endl;
//      if (set_voltage(pid_voltage_offset - nextVoltage.first, 0)) return; // +ve voltage to the left
//      if (set_voltage(pid_voltage_offset + nextVoltage.first, 1)) return; // -ve voltage to the right
//  }

//  if (nextVoltage.second > 0) { // We need to go down
//      std::cout << "Up: " << pid_voltage_offset - nextVoltage.second  << ", down: " << pid_voltage_offset + nextVoltage.second << std::endl;
//      if (set_voltage(-pid_voltage_offset + nextVoltage.second, 3)) return; // -ve voltage down
//      if (set_voltage(-pid_voltage_offset - nextVoltage.second, 2)) return; // +ve voltage up
//  } else { // We need to go up
//      std::cout << "Up: " << pid_voltage_offset - nextVoltage.second << ", down: " << pid_voltage_offset + nextVoltage.second << std::endl;
//      if (set_voltage(-pid_voltage_offset - nextVoltage.second, 2)) return; // -ve voltage up
//      if (set_voltage(-pid_voltage_offset + nextVoltage.second, 3)) return; // +ve voltage down
//  }

  if (set_voltage(pid_voltage_offset - nextVoltage.first, 0)) return; // Left
  if (set_voltage(pid_voltage_offset + nextVoltage.first, 1)) return; // Right
  if (set_voltage(-pid_voltage_offset + nextVoltage.second, 2)) return; // Up
  if (set_voltage(-pid_voltage_offset - nextVoltage.second, 3)) return; // Down
  std::cout << std::endl;
}

void communicate_with_io::one_pole_update_voltages (std::pair < int, double > nextVoltage) {
  switch (nextVoltage.first) {
    case 0:
      if (set_voltage(nextVoltage.second + opc_voltage_offset, 0)) return;
      if (set_voltage(0, 1)) return;
      if (set_voltage(0, 2)) return;
      if (set_voltage(0, 3)) return;
      std::cout << std::endl;
      break;
    case 1:
      if (set_voltage(nextVoltage.second + opc_voltage_offset, 1)) return;
      if (set_voltage(0, 0)) return;
      if (set_voltage(0, 2)) return;
      if (set_voltage(0, 3)) return;
      std::cout << std::endl;
      break;
    case 2:
      if (set_voltage(nextVoltage.second + opc_voltage_offset, 2)) return;
      if (set_voltage(0, 0)) return;
      if (set_voltage(0, 1)) return;
      if (set_voltage(0, 3)) return;
      std::cout << std::endl;
      break;
    case 3:
      if (set_voltage(nextVoltage.second + opc_voltage_offset, 3)) return;
      if (set_voltage(0, 0)) return;
      if (set_voltage(0, 1)) return;
      if (set_voltage(0, 2)) return;
      std::cout << std::endl;
      break;
  }
}
