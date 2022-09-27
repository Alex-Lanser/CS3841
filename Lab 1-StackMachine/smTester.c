#include <stdio.h>
#include "stackm.h"

int main() {
    stackm mystack;
    smInit(&mystack);

    smPush(&mystack, 2);
    smPush(&mystack, 3);
    smPush(&mystack, 4);
    printf("%s\n", "Current Stack:");
    smPrint(&mystack);
    smPop(&mystack);
    printf("%s\n", "Current Stack after Pop:");
    smPrint(&mystack);
    smAdd(&mystack);
    printf("%s\n", "Current Stack after Adding:");
    smPrint(&mystack);

    int value = 0;
    smTop(&mystack, &value);
    printf("%s\n", "Value at top of stack:");
    printf("%d\n", value);

    smPush(&mystack, 10);
    smPush(&mystack, 11);
    printf("%s\n", "Current Stack after Push:");
    smPrint(&mystack);
    smMult(&mystack);
    printf("%s\n", "Current Stack after Multiply:");
    smPrint(&mystack);

    smPush(&mystack, 10);
    smPush(&mystack, 11);
    printf("%s\n", "Current Stack after Push:");
    smPrint(&mystack);
    int rotateValue = 5;
    smRotate(&mystack, rotateValue);
    printf("%s %i%s\n", "Current Stack after Rotation of",rotateValue,":");
    smPrint(&mystack);

    smClear(&mystack);
    printf("%s\n", "Current Stack after Clear:");
    smPrint(&mystack);
    return 0;
}