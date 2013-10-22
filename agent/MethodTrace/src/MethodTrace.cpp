/*
 * MethodTrace.cpp
 *
 *  Created on: 2013/10/22
 *      Author: wtnbsts
 */

#include <jvmti.h>
#include <iostream>
#include <string>

using namespace std;

int main(){
	cout << "mogemoge" << endl;
	return 0;
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved){
	cout << "Agent OnLoad!!" << endl;
}

JNIEXPORT void JNICALL oldAgent_OnUnload(JavaVM* vm){
	cout << "Agent UnLoad!!" << endl;
}

