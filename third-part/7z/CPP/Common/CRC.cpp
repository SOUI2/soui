// Common/CRC.cpp



#include "../../C/7zCrc.h"

struct CCRCTableInit { CCRCTableInit() { CrcGenerateTable(); } } g_CRCTableInit;
