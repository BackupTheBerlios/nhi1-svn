// Created by cgo - DO NOT EDIT

//line factory.go:1
package gomsgque


import (
	"unsafe"
)


type FactoryF func(*MqS) *MqS


var lockFactory = make(map[*FactoryF]bool)


func iErrorFactoryToGoWithCheck(ret uint32) {
	if ret != _Cconst_MQ_FACTORY_RETURN_OK {
		panic(_Cfunc_GoString(_Cfunc_MqFactoryErrorMsg(ret)))
	}
}


func cFactoryCall(tmpl *MqS, cb *FactoryF) (ret *MqS) {
	defer func() {
		if tmpl != nil {
			if x := recover(); x != nil {
				tmpl.ErrorSet(x)
				ret = nil
			}
		}
	}()
	if cb == nil {
		ret = NewMqS(tmpl, nil)
	} else {
		ret = (*cb)(tmpl)
	}
	return
}


func cFactoryDelete(this *MqS, chnp *chan bool) {
	ctxlock[this] = nil, false
	if chnp != nil {
		(*chnp) <- true
	}
}


func cFactoryFree(cb *FactoryF) {
	lockFactory[cb] = false, false
}


func (this *MqS) FactoryCtxIdentGet() string {
	return _Cfunc_GoString(_Cfunc_MqFactoryCtxIdentGet((*_Ctype_struct_MqS)(this)))
}

func (this *MqS) FactoryCtxIdentSet(val string) {
	v := _Cfunc_CString(val)
	r := _Cfunc_MqFactoryCtxIdentSet((*_Ctype_struct_MqS)(this), v)
	_Cfunc_free(unsafe.Pointer(v))
	this.iErrorMqToGoWithCheck(r)
}

func (this *MqS) FactoryCtxDefaultSet(ident string) {
	v := _Cfunc_CString(ident)
	r := _Cfunc_MqFactoryCtxDefaultSet((*_Ctype_struct_MqS)(this), v)
	_Cfunc_free(unsafe.Pointer(v))
	this.iErrorMqToGoWithCheck(r)
}


func FactoryAdd(ident string, cb FactoryF) {
	v := _Cfunc_CString(ident)
	r := _Cfunc_gomsgque_FactoryAdd(v, _Ctypedef_MQ_PTR(&cb))
	_Cfunc_free(unsafe.Pointer(v))
	iErrorFactoryToGoWithCheck(r)
	lockFactory[&cb] = true
}

func FactoryDefault(ident string, cb FactoryF) {
	v := _Cfunc_CString(ident)
	r := _Cfunc_gomsgque_FactoryDefault(v, _Ctypedef_MQ_PTR(&cb))
	_Cfunc_free(unsafe.Pointer(v))
	iErrorFactoryToGoWithCheck(r)
	lockFactory[&cb] = true
}

func FactoryDefaultIdent() string {
	return _Cfunc_GoString(_Cfunc_MqFactoryDefaultIdent())
}

func FactoryCall(ident string) *MqS {
	v := _Cfunc_CString(ident)
	out := _Cfunc_gomsgque_FactoryCall(v)
	_Cfunc_free(unsafe.Pointer(v))
	iErrorFactoryToGoWithCheck(out.ret)
	return (*MqS)(out.ctx)
}

func FactoryNew(ident string, cb FactoryF) *MqS {
	v := _Cfunc_CString(ident)
	out := _Cfunc_gomsgque_FactoryNew(v, _Ctypedef_MQ_PTR(&cb))
	_Cfunc_free(unsafe.Pointer(v))
	iErrorFactoryToGoWithCheck(out.ret)
	lockFactory[&cb] = true
	return (*MqS)(out.ctx)
}
