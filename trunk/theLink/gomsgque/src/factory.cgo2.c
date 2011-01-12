#include <gomsgque.h>



// Usual nonsense: if x and y are not equal, the type will be invalid
// (have a negative array count) and an inscrutable error will come
// out of the compiler and hopefully mention "name".
#define __cgo_compile_assert_eq(x, y, name) typedef char name[(x-y)*(x-y)*-2+1];

// Check at compile time that the sizes we use match our expectations.
#define __cgo_size_assert(t, n) __cgo_compile_assert_eq(sizeof(t), n, _cgo_sizeof_##t##_is_not_##n)

__cgo_size_assert(char, 1)
__cgo_size_assert(short, 2)
__cgo_size_assert(int, 4)
typedef long long __cgo_long_long;
__cgo_size_assert(__cgo_long_long, 8)
__cgo_size_assert(float, 4)
__cgo_size_assert(double, 8)

#include <errno.h>
#include <string.h>

void
_cgo_be8099d53211_Cfunc_MqFactoryErrorMsg(void *v)
{
	struct {
		enum MqFactoryReturnE p0;
		char __pad4[4];
		MQ_CST r;
	}
 *a = v;
	a->r = MqFactoryErrorMsg(a->p0);
}

void
_cgo_be8099d53211_Cfunc_MqFactoryDefaultIdent(void *v)
{
	struct {
		MQ_CST r;
	}
 *a = v;
	a->r = MqFactoryDefaultIdent();
}

void
_cgo_be8099d53211_Cfunc_gomsgque_FactoryDefault(void *v)
{
	struct {
		MQ_CST p0;
		MQ_PTR p1;
		enum MqFactoryReturnE r;
		char __pad20[4];
	}
 *a = v;
	a->r = gomsgque_FactoryDefault(a->p0, a->p1);
}

void
_cgo_be8099d53211_Cfunc_MqFactoryCtxIdentGet(void *v)
{
	struct {
		struct MqS* p0;
		MQ_CST r;
	}
 *a = v;
	a->r = MqFactoryCtxIdentGet(a->p0);
}

void
_cgo_be8099d53211_Cfunc_gomsgque_FactoryCall(void *v)
{
	struct {
		MQ_CST p0;
		struct FactoryCallReturn r;
	}
 *a = v;
	a->r = gomsgque_FactoryCall(a->p0);
}

void
_cgo_be8099d53211_Cfunc_gomsgque_FactoryNew(void *v)
{
	struct {
		MQ_CST p0;
		MQ_PTR p1;
		struct FactoryCallReturn r;
	}
 *a = v;
	a->r = gomsgque_FactoryNew(a->p0, a->p1);
}

void
_cgo_be8099d53211_Cfunc_MqFactoryCtxDefaultSet(void *v)
{
	struct {
		struct MqS* p0;
		MQ_CST p1;
		enum MqErrorE r;
		char __pad20[4];
	}
 *a = v;
	a->r = MqFactoryCtxDefaultSet(a->p0, a->p1);
}

void
_cgo_be8099d53211_Cfunc_gomsgque_FactoryAdd(void *v)
{
	struct {
		MQ_CST p0;
		MQ_PTR p1;
		enum MqFactoryReturnE r;
		char __pad20[4];
	}
 *a = v;
	a->r = gomsgque_FactoryAdd(a->p0, a->p1);
}

void
_cgo_be8099d53211_Cfunc_MqFactoryCtxIdentSet(void *v)
{
	struct {
		struct MqS* p0;
		char* p1;
		enum MqErrorE r;
		char __pad20[4];
	}
 *a = v;
	a->r = MqFactoryCtxIdentSet(a->p0, a->p1);
}

