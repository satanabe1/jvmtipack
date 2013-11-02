/*
 * MethodTrace.c
 *
 *  Created on: 2013/10/22
 *      Author: wtnbsts
 */

#include <jvmti.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

// CHECK_****_RESULT はjvmti関係の関数の戻り値が，エラーかどうかを判定する
#ifndef CHECK_JVMTI_RESULT
#define CHECK_JVMTI_RESULT(caption, error) \
	if (error != JVMTI_ERROR_NONE) { \
		cerr <<"ERROR["<<__FILE__<<":" << __LINE__<<"]"<<caption<< endl ;\
	return error;}
#endif
#ifndef CHECK_JNI_RESULT
#define CHECK_JNI_RESULT(caption, error) \
	if (error != JNI_OK) { \
		cerr <<"ERROR["<<__FILE__<<":" << __LINE__<<"]"<<caption<< endl ;\
	return error;}
#endif

/**
 * Agent_OnLoad関数の "callbacks.ClassFileLoadHook = &classFileLoadHook;" でコールバックとして登録されている
 */
static void classFileLoadHook(jvmtiEnv *jvmti_env, JNIEnv* jni_env,
		jclass class_being_redefined, jobject loader, const char* name,
		jobject protection_domain, jint class_data_len,
		const unsigned char* class_data, jint* new_class_data_len,
		unsigned char** new_class_data) {
	cout << "file load " << name << endl;
}

static void classLoad(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread,
		jclass klass) {
	cout << "load" << endl;
}

static void classPrepare(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread,
		jclass klass) {
	cout << "prepare" << endl;
}

/**
 * JVMTIエージェントがロードされた際に呼び出される
 * この関数でエージェントの権限の設定やコールバックの設定を行う
 * @param [in] vm
 * @param [in] options これは "java -agentlib:AgentName=hogehoge JavaClass" の "hogehoge" に当たる文字列が入る
 * @param [in] reserved
 */
JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved) {
	jvmtiEventCallbacks callbacks;
	jvmtiError error;
	jint result;
	jvmtiEnv* jvmti_env;
	jvmtiCapabilities capa;
	cout << "** Agent_OnLoad" << endl;
	cout << "** options " << options << endl;

	// jvmtiの構造体的なものを取得する
	result = vm->GetEnv((void**) &jvmti_env, JVMTI_VERSION_1_2);
	if (result != JNI_OK || jvmti_env == NULL) {
		CHECK_JNI_RESULT("vm-GetEnv", JVMTI_ERROR_INTERNAL); // エラー処理
	}

	// エージェントの権限を初期化する
	memset(&capa, JVMTI_DISABLE, sizeof(jvmtiCapabilities));
	// エージェントに権限を設定する // 今回はあまり意味が無い
	error = jvmti_env->AddCapabilities(&capa);
	CHECK_JVMTI_RESULT("jvmti->AddCapabilities", error); // エラー処理

	// コールバック関数テーブルを初期化する
	memset(&callbacks, JVMTI_DISABLE, sizeof(jvmtiEventCallbacks));
	// コールバック関数テーブルに関数を登録する
	callbacks.ClassFileLoadHook = &classFileLoadHook;
	callbacks.ClassLoad = &classLoad;
	callbacks.ClassPrepare = &classPrepare;

	// コールバック用のイベントの発生を有効化する // これをしないと，コールバック関数が実行されない
	jvmti_env->SetEventNotificationMode(JVMTI_ENABLE,
			JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,
			NULL);
	jvmti_env->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_LOAD,
	NULL);
	jvmti_env->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE,
	NULL);

	// コールバック関数テーブルをエージェントにセットする
	error = jvmti_env->SetEventCallbacks(&callbacks, (jint) sizeof(callbacks));
	CHECK_JVMTI_RESULT("jvmti->SetEventCallbacks", error); // エラー処理

	return JNI_OK;
}

/**
 * VMが終了する際に呼び出される
 */
JNIEXPORT void JNICALL Agent_OnUnload(JavaVM* vm) {
	cout << "** Agent_OnUnload" << endl;
}

