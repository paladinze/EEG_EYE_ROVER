
#include "Utility.h"

void print_usage() {
	const char * cmd_usage = "\tusage: %s -d <devicename> -b <baudrate> [-v/--verbose] [--debug]\n\t\tdefault: -d %s -b %i\n";
	printf("%s", cmd_usage);

}