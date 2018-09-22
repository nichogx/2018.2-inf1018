#include "grava_structs.h"

int main(void) {
	struct s {
		int i1;
		long long l1;
		short s1;
		char c1;
		short s2;
	};
	struct s exemplo[10];
	
	exemplo[0].i1 = 0x01020304;
	exemplo[0].l1 = 0x0102030405060708;
	exemplo[0].s1 = 0x0102;
	exemplo[0].c1 = 0xff;
	exemplo[0].s2 = 0xaabb;
	
	grava_structs(10, exemplo, "ilscs", 'L', "saida1");

	return 0;
}
