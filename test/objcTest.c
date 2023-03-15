#include <macrOS/macrOSObjC.h>
#include <stdlib.h>

#define CORRECT_VAL 1234

int returnVal(id self, SEL sel) {
    return CORRECT_VAL;
}

int main(int argc, char** argv) {
    initObjCRuntime();
    /* Create an Objective-C class */
    Class TestClass = makeClass("TestClass", "NSObject");
    /* Add a method to the class */
    addMethod(TestClass, "returnVal:", returnVal, "i@:");
    /* Register the class */
    finaliseClass(TestClass);
    /* Create an instance of the class */
    id testClassInstance = objc_alloc_init(TestClass);
    /* Call a method */
    int returnedValue = objc_msgSend_t(int)(testClassInstance, sel("returnVal:"));
    /* Release the instance */
    objc_msgSend_id(testClassInstance, sel("release"));
    /* Check the return value */
    return (returnedValue == CORRECT_VAL) ? EXIT_SUCCESS : EXIT_FAILURE;
}
