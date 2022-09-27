#include <stdio.h>
#include "stackm.h"

int main() {
	stackm mystack;
	smInit(&mystack);
	printf("SIZE: %d\n", smSize(&mystack));
	return 0;
}
