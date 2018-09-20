#include "grava_structs.h"

int main(void) {
	struct s {
		int i1;
		long l1;
		short s1;
		char c1;
		short s2;
	};
	struct s exemplo[10];
	
	grava_structs(10, exemplo, "ilscs", 'L', "saida1");

	return 0;
}
