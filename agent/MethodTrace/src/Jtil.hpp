/*
 * Jtil.hpp
 *
 *  Created on: 2013/10/25
 *      Author: wtnbsts
 */

#ifndef JPUTIL_HPP_
#define JPUTIL_HPP_

#include <jvmti.h>

namespace jvmtipack {

class Jtil {
private:
	jvmtiEnv* jvmti;
public:
	Jtil();
	Jtil(jvmtiEnv* jvmti) {
		this->jvmti = jvmti;
	}
	;

	jvmtiError GetMethodKey(jmethodID method, char** key) {
		jclass klass = NULL;
		char* csig;
		char* gcsig;
//		(&jvmti)->GetMethodDeclaringClass(method, &klass);
		jvmti->GetMethodDeclaringClass(method, &klass);
		jvmti->GetClassSignature(klass,&csig, &gcsig);

		jvmti->GetClassSignature(klass,key, &gcsig);

		return JVMTI_ERROR_NONE;
	}
	;
};

} /* namespace jvmtipack */

#endif /* JPUTIL_HPP_ */
