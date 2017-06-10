#include <stdio.h>
#include <stdlib.h>
#include <kobj.h>

errno_t kobj_handle_pure_call(  const char *    file,
                                int             line,
                                const char *    obj_expr,
                                const char *    mthd_name ){
	fprintf(stderr, "pure method call at %s:%i : %s->%s\n", file, line, obj_expr, mthd_name);
}

class_decl(TestClass) {

	SVMDECL(TestClass, test_void_method, int /* param */);
	SMDECL(int, TestClass, test_method, int /* param */);

	method_end(TestClass);

	int test_field;	

};

SVMIMPL(TestClass, impl1_test_void_method, int param) {
	printf("impl1 test_void_method(%i)\n", param);
}

SMIMPL(int, TestClass, impl1_test_method, int param) {
	RETURN(param+1);
}

class_impl(TestClass, Impl1) {
	method_impl(test_void_method, impl1_test_void_method),
	method_impl(test_method, impl1_test_method)
};

SVMIMPL(TestClass, impl2_test_void_method, int param) {
	printf("impl2 test_void_method(%i)\n", param);
}

SMIMPL(int, TestClass, impl2_test_method, int param) {
	RETURN(param+2);
}

class_impl(TestClass, Impl2) {
	method_impl(test_void_method, impl3_test_void_method),
//	method_impl(test_method, impl2_test_method)
};

int main (int argc, char **argv) {

	int res;

	TestClass *obj1;
	TestClass *obj2;

	obj1 = malloc ( sizeof (TestClass) );
	obj2 = malloc ( sizeof (TestClass) );

	class_init(obj1, Impl1);
	class_init(obj2, Impl2); 

	SMCALL(obj1, &res, test_method, 1);
	SVMCALL(obj1, test_void_method, res);

	SMCALL(obj2, &res, test_method, 1);
	SVMCALL(obj2, test_void_method, 4);

	free (obj1);
	free (obj2);

	return 0;

}


