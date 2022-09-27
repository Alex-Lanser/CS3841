#include <stdio.h>
#include "stackm.h"

void doPop(stackm* s) 
{
	printf("Pop Ret: %d\n", smPop(s));
}

int main() {
	stackm mystack;
	smInit(&mystack);
	doPop(&mystack);
	return 0;
}
