/*
 * MethodTrace.c
 *
 *  Created on: 2013/10/22
 *      Author: wtnbsts
 */

#include <jvmti.h>
#include <stdio.h>
#include <string.h>

static char* LOG_FILE = "log.log";
static char* PACKAGE_FILTER = "jv"; /* このパッケージだけトレースする */

static FILE *fp;

static jvmtiEnv* jvmti = NULL;
static jvmtiCapabilities capa;

static void putMethodCallInfo(jvmtiEnv* jvmti, JNIEnv* env, jthread thread,
		jmethodID method, char* direction) {
	jvmtiError error;
	char* name = NULL;
	char* msig = NULL;
	char* gmsig = NULL;
	char* csig = NULL;
	char* gcsig = NULL;
	jclass clazz;
	jvmtiThreadInfo threadInfo;

	threadInfo.name = NULL;

	/* メソッドの属するクラス */
	error = (*jvmti)->GetMethodDeclaringClass(jvmti, method, &clazz);
	if (error != JVMTI_ERROR_NONE) {
		printf("GetMethodDeclaringClass:%d\n", error);
		goto exit;
	}

	/* クラスのシグニチャ */
	error = (*jvmti)->GetClassSignature(jvmti, clazz, &csig, &gcsig);
	if (error != JVMTI_ERROR_NONE) {
		printf("GetClassSignature:%d\n", error);
		goto exit;
	}

	/* パッケージ名にPACKAGE_FILTERがが含まれていない */
	if (strstr(csig, PACKAGE_FILTER) == NULL) {
		goto exit;
	} else {
		//fprintf(fp,"%s\n", csig);
		//printf("%s\n", csig);
		//printf("HITTTTTT!");
	}

	/* メソッド名、シグニチャ */
	error = (*jvmti)->GetMethodName(jvmti, method, &name, &msig, &gmsig);
	if (error != JVMTI_ERROR_NONE) {
		printf("GetMethodName:%d\n", error);
		goto exit;
	}

	if (strstr(name, "<clinit>") != NULL) {
		goto exit;
	} else if (strstr(name, "<init>") != NULL) {
		goto exit;
	}

	/* スレッド情報 */
	error = (*jvmti)->GetThreadInfo(jvmti, thread, &threadInfo);
	if (error != JVMTI_ERROR_NONE) {
		printf("GetThreadInfo:%d\n", error);
		goto exit;
	}

	printf("------------------\n");
	//printf("ClassLoader : \n", threadInfo.context_class_loader);
	printf("Direction : %s\n", direction);
	printf("Name      : %s\n", name);
	fprintf(fp, "%s,%s,%s,%s,%s\n", threadInfo.name, direction, csig, name,
			msig);

	exit: if (csig != NULL) {
		(*jvmti)->Deallocate(jvmti, csig);
	}
	if (gcsig != NULL) {
		(*jvmti)->Deallocate(jvmti, gcsig);
	}
	if (msig != NULL) {
		(*jvmti)->Deallocate(jvmti, msig);
	}
	if (gmsig != NULL) {
		(*jvmti)->Deallocate(jvmti, gmsig);
	}
	if (threadInfo.name != NULL) {
		(*jvmti)->Deallocate(jvmti, threadInfo.name);
	}
}

/**
 * JavaVM上でメソッドが実行されたら呼び出される。
 *
 */
static void JNICALL oldmethodEntry(jvmtiEnv* jvmti, JNIEnv* env, jthread thread,
		jmethodID method) {
	putMethodCallInfo(jvmti, env, thread, method, ">");
}

/**
 * JavaVM上でメソッドが実行されたら呼び出される。
 *
 */
static void JNICALL oldmethodExit(jvmtiEnv *jvmti, JNIEnv* env, jthread thread,
		jmethodID method, jboolean was_popped_by_exception, jvalue return_value) {

	if (was_popped_by_exception == JNI_FALSE) {
		putMethodCallInfo(jvmti, env, thread, method, "<");
	}
}

/**
 * JavaVMが初期化される際に呼び出されるコールバック関数。
 * Agent_OnLoad内でコールバックとして登録している。
 */
static void JNICALL oldvmInit(jvmtiEnv* jvmti, JNIEnv* env, jthread thread) {
	fp = fopen(LOG_FILE, "w");

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
JNIEXPORT jint JNICALL oldAgent_OnLoad(JavaVM* vm, char* options,
		void* reserved) {
	jvmtiError error;
	jint ret;
	jvmtiEventCallbacks callbacks;

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
	callbacks.VMInit = &oldvmInit;
	callbacks.MethodEntry = &oldmethodEntry;
	callbacks.MethodExit = &oldmethodExit;
	(*jvmti)->SetEventCallbacks(jvmti, &callbacks, sizeof(callbacks));

	(*jvmti)->SetEventNotificationMode(jvmti, JVMTI_ENABLE, JVMTI_EVENT_VM_INIT,
	NULL);

	return JNI_OK;
}

/**
 * JavaVMが終了する際に呼び出される。
 */
 JNIEXPORT void JNICALL oldAgent_OnUnload(JavaVM* vm) {
	printf("Good-bye, JVMTI.\n");

	fclose(fp);
}

