/*
 * AgentSeed.cpp
 *
 *  Created on: 2013/10/22
 *      Author: wtnbsts
 */

#include <jvmti.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "jtil.hpp"

#define DEBUG_METHOD
#define DEBUG_FIELD_
#define DEBUG_CLASS_

#define EXCLUDES(jvmti,method) if (excludes(jvmti, method)) {return;}

using namespace std;
using namespace jvmtipack;

//static vector<string> log;
static const jvmtiError &ERROR_NONE = JVMTI_ERROR_NONE;
static int nest = 0;

static bool excludes(jvmtiEnv* jvmti, jmethodID method) {
	jvmtiError error;
	jclass clazz;
	string classSignature;
	string::size_type index;
	string padding;
	char* csig = NULL;
	char* gcsig = NULL;
	char* name = NULL;
	char* msig = NULL;
	char* gmsig = NULL;
//	int i;

	error = jvmti->GetMethodDeclaringClass(method, &clazz);
	if (error != ERROR_NONE) {
		cerr << "in [static bool excludes(jvmtiEnv* jvmti, jmethodID method)]"
				<< endl;
		cerr << "jvmti->GetMethodDeclaringClass" << endl;
		return true;
	}
	error = jvmti->GetClassSignature(clazz, &csig, &gcsig);
	//	if (...){cerr}
	classSignature = string(csig);
	index = classSignature.find("Ljava/");
	if (index == 0) {
		return true;
	}
	index = classSignature.find("Lsun/");
	if (index == 0) {
		return true;
	}

	error = jvmti->GetMethodName(method, &name, &msig, &gmsig);
	//	if (...){cerr}

	return false;
}

/**
 * クラスが使えるようになったら呼ばれるコールバック
 *
 */
static void JNICALL classPrepare(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread,
		jclass klass) {
	char* csig = NULL;
	char* gcsig = NULL;
	jint count;
	jfieldID* fields;
	jvmtiError error;
	int i;

	jvmti->GetClassSignature(klass, &csig, &gcsig);
	error = jvmti->GetClassFields(klass, &count, &fields);
	if (error != ERROR_NONE) {
		cerr << "errror: GetClassFields:" << error << endl;
	}

#ifdef DEBUG_CLASS
	cout << "CPrepare[" << csig << "] field:" << count << endl;
#endif

	for (i = 0; i < count; i++) {
		error = jvmti->SetFieldAccessWatch(klass, fields[i]);
		if (error != ERROR_NONE) {
			cerr << "error: SetFieldAccessWatch:" << error << endl;
		} else {
//			cout << "ok:" << endl;
		}
		error = jvmti->SetFieldModificationWatch(klass, fields[i]);
		if (error != ERROR_NONE) {
			cerr << "error: SetFieldModification:" << error << endl;
		} else {
//			cout << "ok:" << endl;
		}
	}
}

static void JNICALL fieldAccess(jvmtiEnv *jvmti, JNIEnv* jni, jthread thread,
		jmethodID method, jlocation location, jclass field_klass,
		jobject object, jfieldID field) {
	EXCLUDES(jvmti, method);

#ifdef DEBUG_FIELD
	char* fname = NULL;
	char* fsig = NULL;
	char* fgsig = NULL;
	jvmti->GetFieldName(field_klass, field, &fname, &fsig, &fgsig);
	cout << "FAccess:" << fname << endl;
#endif
}

static void JNICALL fieldModification(jvmtiEnv *jvmti, JNIEnv* jni,
		jthread thread, jmethodID method, jlocation location,
		jclass field_klass, jobject object, jfieldID field, char signature_type,
		jvalue new_value) {
	EXCLUDES(jvmti, method);

#ifdef DEBUG_FIELD
	char* fname = NULL;
	char* fsig = NULL;
	char* fgsig = NULL;
	jvmti->GetFieldName(field_klass, field, &fname, &fsig, &fgsig);
	cout << "FModification:" << fname << endl;
#endif
}

/**
 * JavaVM上でメソッドが実行されたら呼び出される。
 *
 */
static void JNICALL methodEntry(jvmtiEnv* jvmti, JNIEnv* env, jthread thread,
		jmethodID method) {
//	jvmtiError error;
	nest++;
	EXCLUDES(jvmti, method);

	char* key;
	Jtil jtil = Jtil(jvmti);

	jtil.GetMethodKey(method, &key);
	cout << "KEY:" << key << endl;

#ifdef DEBUG_METHOD
	char* mname;
	char* msig;
	char* gmsig;
	jclass klass;
	char* csig;
	char* gcsig;
	string padding;
	int i;
	for (i = 0; i < nest; i++) {
		padding += "  ";
	}
	jvmti->GetMethodName(method, &mname, &msig, &gmsig);
	jvmti->GetMethodDeclaringClass(method, &klass);
	jvmti->GetClassSignature(klass, &csig, &gcsig);
	cout << padding << "+ " << csig << "." << mname << endl;
#endif

//	jint freadDep;
//	error = jvmti->GetFrameCount(thread, &freadDep);
//	cout << error << endl;
//	cout << freadDep << endl;
//	jvmti->GetLocal

}

/**
 * JavaVM上でメソッドが実行されたら呼び出される。
 *
 */
static void JNICALL methodExit(jvmtiEnv *jvmti, JNIEnv* env, jthread thread,
		jmethodID method, jboolean was_popped_by_exception,
		jvalue return_value) {
	nest--;
	EXCLUDES(jvmti, method);

#ifdef DEBUG_METHOD
	char* mname;
	char* msig;
	char* gmsig;
	jclass klass;
	char* csig;
	char* gcsig;
	string padding;
	int i;
	for (i = 0; i < nest + 1; i++) {
		padding += "  ";
	}
	jvmti->GetMethodName(method, &mname, &msig, &gmsig);
	jvmti->GetMethodDeclaringClass(method, &klass);
	jvmti->GetClassSignature(klass, &csig, &gcsig);
	cout << padding << "- " << csig << "." << mname << endl;
#endif
}

static void JNICALL vmInit(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
	cout << "*** VM Initialize" << endl;
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved) {
	jvmtiEventCallbacks callbacks;
	jvmtiError error;
	jint result;
	jvmtiEnv* jvmti;
	jvmtiCapabilities capa;
	cout << "*** Agent OnLoad!!" << endl;

	result = vm->GetEnv((void**) &jvmti, JVMTI_VERSION_1_2);
	if (result != JNI_OK || jvmti == NULL) {
		cerr << "ERROR: vm->GetEnv" << endl;
		cerr << "       result   = " << result << endl;
		cerr << "       jvmti = " << jvmti << endl;
		return JNI_ERR;
	}

	memset(&capa, JVMTI_DISABLE, sizeof(jvmtiCapabilities));
	capa.can_generate_method_entry_events = JVMTI_ENABLE;
	capa.can_generate_method_exit_events = JVMTI_ENABLE;
	capa.can_generate_field_access_events = JVMTI_ENABLE;
	capa.can_generate_field_modification_events = JVMTI_ENABLE;
	capa.can_access_local_variables = JVMTI_ENABLE;

	error = jvmti->AddCapabilities(&capa);
	if (error != ERROR_NONE) {
		cerr << "ERROR: jvmti->AddCapabilities" << endl;
		cerr << "       error   = " << error << endl;
	}

	memset(&callbacks, 0, sizeof(jvmtiEventCallbacks));
	callbacks.VMInit = &vmInit;
	callbacks.MethodEntry = &methodEntry;
	callbacks.MethodExit = &methodExit;
	callbacks.ClassPrepare = &classPrepare;
	callbacks.FieldAccess = &fieldAccess;
	callbacks.FieldModification = &fieldModification;

	jvmti->SetEventCallbacks(&callbacks, (jint) sizeof(callbacks));
	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL);
	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY,
	NULL);
	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_EXIT,
	NULL);
//	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_SINGLE_STEP,
//	NULL);
	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE,
	NULL);
	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_FIELD_ACCESS,
	NULL);
	jvmti->SetEventNotificationMode(JVMTI_ENABLE,
			JVMTI_EVENT_FIELD_MODIFICATION,
			NULL);
	return JNI_OK;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm) {
	cout << "*** Agent UnLoad!!" << endl;
	cout << "-------LOG------------" << endl;
	return;
//vector<string>::iterator it = log.begin();
//while (it != log.end()) {
//	cout << *it << endl;
//	++it;
//}
}

