/*
 * MethodTrace.cpp
 *
 *  Created on: 2013/10/22
 *      Author: wtnbsts
 */

#include <jvmti.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

//static jvmtiEnv* jvmti = NULL;
static vector<string> log;
static const jvmtiError &ERROR_NONE = JVMTI_ERROR_NONE;
static int nest = 0;

static void pritMethodCallInfo(jvmtiEnv* jvmti, JNIEnv* env, jthread thread,
		jmethodID method) {
	jclass clazz;
	jvmtiThreadInfo threadInfo;
	threadInfo.name = NULL;
	cout << "print method0" << endl;
}

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
	int i;

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

	for (i = 0; i < nest; i++) {
		padding += "  ";
	}
	cout << padding << classSignature << "." << name << endl;
	return true;
}

/**
 * JavaVM上でメソッドが実行されたら呼び出される。
 *
 */
static void JNICALL mymethodEntry(jvmtiEnv* jvmti, JNIEnv* env, jthread thread,
		jmethodID method) {
	static int count = 0;
	stringstream ss;
	nest++;
	count++;
	if (excludes(jvmti, method)) {
		return;
	}
	ss << "MEntry[" << count << "]";
	log.push_back(ss.str());

}

/**
 * JavaVM上でメソッドが実行されたら呼び出される。
 *
 */
static void JNICALL methodExit(jvmtiEnv *jvmti, JNIEnv* env, jthread thread,
		jmethodID method, jboolean was_popped_by_exception,
		jvalue return_value) {
	nest--;
	if (was_popped_by_exception == JNI_FALSE) {
	}
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

	memset(&capa, 0, sizeof(jvmtiCapabilities));
	capa.can_generate_method_entry_events = JVMTI_ENABLE;
	capa.can_generate_method_exit_events = JVMTI_ENABLE;
	error = jvmti->AddCapabilities(&capa);
	if (error != ERROR_NONE) {
		cerr << "ERROR: jvmti->AddCapabilities" << endl;
		cerr << "       error   = " << error << endl;
	}

	memset(&callbacks, 0, sizeof(jvmtiEventCallbacks));
	callbacks.VMInit = &vmInit;
	callbacks.MethodEntry = &mymethodEntry;
	callbacks.MethodExit = &methodExit;

	jvmti->SetEventCallbacks(&callbacks, (jint) sizeof(callbacks));
	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_VM_INIT, NULL);
	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_ENTRY,
	NULL);
	jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_METHOD_EXIT,
	NULL);
	return JNI_OK;
}

JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm) {
	cout << "*** Agent UnLoad!!" << endl;
	cout << "-------LOG------------" << endl;
	vector<string>::iterator it = log.begin();
	while (it != log.end()) {
		cout << *it << endl;
		++it;
	}
}

