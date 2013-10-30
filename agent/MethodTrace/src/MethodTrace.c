/*
 * MethodTrace.c
 *
 *  Created on: 2013/10/22
 *      Author: wtnbsts
 */

#include <jvmti.h>
#include <stdio.h>
#include <string.h>

static FILE *fp;

/**
 * JavaVM上でメソッドが実行されたら呼び出される。
 *
 */
static void JNICALL MethodEntry(jvmtiEnv* jvmti, JNIEnv* env, jthread thread,
		jmethodID method) {
}

/**
 * JavaVM上でメソッドが実行されたら呼び出される。
 *
 */
static void JNICALL MethodExit(jvmtiEnv *jvmti, JNIEnv* env, jthread thread,
		jmethodID method, jboolean was_popped_by_exception, jvalue return_value) {

	if (was_popped_by_exception == JNI_FALSE) {
	}
}

/**
 * JavaVMが初期化される際に呼び出されるコールバック関数。
 * Agent_OnLoad内でコールバックとして登録している。
 */
static void JNICALL vmInit(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {

	printf("JVMTI_EVENT_VM_INIT notified\n");

	(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,
			JVMTI_EVENT_METHOD_ENTRY, NULL);
	(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE,
			JVMTI_EVENT_METHOD_EXIT, NULL);
}

/**
 * JVMTIエージェントが含まれたライブラリがロードされた際に呼び出される。
 * options に'abort'が指定されると、JNI_ERRを返却する
 */
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved) {
	jvmtiError error;
	jint ret;
	jvmtiEventCallbacks callbacks;
	jvmtiCapabilities capa;
	jvmtiEnv* jvmti;

	printf("Hello, JVMTI. option '%s' specified\n", options);

	ret = (*vm)->GetEnv(vm, (void**) &jvmti, JVMTI_VERSION_1_0);
	if (ret != JNI_OK || jvmti == NULL) {
		printf("ERROR: Unable to access JVMTI Version 1 (0x%x),"
				"is your J2SE a 1.5 or newer version?"
				"JNIEnv's GetEnv() returned %d\n", JVMTI_VERSION_1, ret);
	}

	(void) memset(&capa, 0, sizeof(jvmtiCapabilities));
	capa.can_generate_method_entry_events = 1;
	capa.can_generate_method_exit_events = 1;
	error = (*jvmti)->AddCapabilities(jvmti, &capa);
	if (error != JVMTI_ERROR_NONE) {
		printf("AddCapabilities:%d\n", error);
		return JNI_ERR;
	}

	memset(&callbacks, 0, sizeof(callbacks));
	callbacks.VMInit = &vmInit;
	callbacks.MethodEntry = &MethodEntry;
	callbacks.MethodExit = &MethodExit;
	(*jvmti)->SetEventCallbacks(jvmti, &callbacks, sizeof(callbacks));

	(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_INIT,
	NULL);

	return JNI_OK;
}

/**
 * JavaVMが終了する際に呼び出される。
 */
JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm) {
	printf("Good-bye, JVMTI.\n");

	fclose(fp);
}

