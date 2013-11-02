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
 * SetFieldAccessWatch で登録されたフィールドへのアクセスでしか，コールバックされない
 */
static void fieldAccess(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread,
		jmethodID method, jlocation location, jclass field_klass,
		jobject object, jfieldID field) {
	char *fname, *fsig, *fgsig;
	jvmti_env->GetFieldName(field_klass, field, &fname, &fsig, &fgsig);
	cout << "FieldAccess " << fname << endl;
}

/**
 * SetFieldModificationWatch で登録されたフィールドの変更でしか，コールバックされない
 */
static void fieldModification(jvmtiEnv *jvmti_env, JNIEnv* jni_env,
		jthread thread, jmethodID method, jlocation location,
		jclass field_klass, jobject object, jfieldID field, char signature_type,
		jvalue new_value) {
	char *fname, *fsig, *fgsig;
	jvmti_env->GetFieldName(field_klass, field, &fname, &fsig, &fgsig);
	cout << "FieldModification " << fname << endl;
}

/**
 * クラスが使えるようになったら，そのクラスのフィールドの一覧を GetClassFields で取得し，
 * 帰ってきたフィールドの一覧に対して SetFieldAccessWatch, SetFieldModificationWatch で，
 * フィールドアクセスと，フィールド変更を監視する
 */
static void classPrepare(jvmtiEnv *jvmti_env, JNIEnv* jni_env, jthread thread,
		jclass klass) {
	cout << "prepare" << endl;
	jint count;
	jfieldID* field;
	int i;
	jvmti_env->GetClassFields(klass, &count, &field);
	for (i = 0; i < count; i++) {
		jvmti_env->SetFieldAccessWatch(klass, field[i]);
		jvmti_env->SetFieldModificationWatch(klass, field[i]);
	}
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
	// 必要な権限を有効化する
	capa.can_generate_field_access_events = JVMTI_ENABLE;
	capa.can_generate_field_modification_events = JVMTI_ENABLE;
	// エージェントに権限を設定する // 今回はあまり意味が無い
	error = jvmti_env->AddCapabilities(&capa);
	CHECK_JVMTI_RESULT("jvmti->AddCapabilities", error); // エラー処理

	// コールバック関数テーブルを初期化する
	memset(&callbacks, JVMTI_DISABLE, sizeof(jvmtiEventCallbacks));
	// コールバック関数テーブルに関数を登録する
	callbacks.ClassPrepare = &classPrepare;
	callbacks.FieldAccess = &fieldAccess;
	callbacks.FieldModification = &fieldModification;

	// コールバック用のイベントの発生を有効化する // これをしないと，コールバック関数が実行されない
	jvmti_env->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_CLASS_PREPARE,
	NULL);
	jvmti_env->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_FIELD_ACCESS,
	NULL);
	jvmti_env->SetEventNotificationMode(JVMTI_ENABLE,
			JVMTI_EVENT_FIELD_MODIFICATION, NULL);

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

