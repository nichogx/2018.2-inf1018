#include "grava_structs.h"

int main(void) {
	struct s {
		int i1;
		long long l1;
		short s1;
		char c1;
		short s2;
	};
	struct s exemplo[3];
	
	exemplo[0].i1 = 0x01020304;
	exemplo[0].l1 = 0x0102030405060708;
	exemplo[0].s1 = 0x0102;
	exemplo[0].c1 = 0xff;
	exemplo[0].s2 = 0xaabb;
	exemplo[1].i1 = 0x01020304;
	exemplo[1].l1 = 0x0102030405060708;
	exemplo[1].s1 = 0x0102;
	exemplo[1].c1 = 0xff;
	exemplo[1].s2 = 0xaabb;
	exemplo[2].i1 = 0xffffffff;
	exemplo[2].l1 = 0xaaaaaaaaaaaaaaaa;
	exemplo[2].s1 = 0xbbbb;
	exemplo[2].c1 = 0xcc;
	exemplo[2].s2 = 0xdddd;
	
	grava_structs(3, exemplo, "ilscs", 'B', "saida1");
	
	dump_structs("saida1");

	return 0;
}
