/*
 * Jtil.hpp
 *
 *  Created on: 2013/10/25
 *      Author: wtnbsts
 */

#ifndef JPUTIL_HPP_
#define JPUTIL_HPP_

#include <jvmti.h>
#include <string>

using namespace std;

//static const jvmtiError &ERROR_NONE = JVMTI_ERROR_NONE;

#ifndef CHECK_RESULT
#define CHECK_RESULT(caption, error) \
	if (error != JVMTI_ERROR_NONE) { \
		cerr <<__FILE__<<"." << __LINE__<<":"<<caption<< endl ;\
	return error;}
#endif

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
	jvmtiError GetClassName(jclass& klass, string& package, string& name) {
		jvmtiError error;
		char *csig, *gcsig;
		string longName;
		string::size_type findIndex;
		error = jvmti->GetClassSignature(klass, &csig, &gcsig);
		CHECK_RESULT("GetClassName", error);
		// csig,longName : "Ljava/lang/String;"
		longName = string(csig);
		findIndex = longName.rfind('/');
		if (findIndex == string::npos) {
			package = "";
			name = longName.substr(1, longName.size() - 2);
			return JVMTI_ERROR_NONE;
		}
		package = longName.substr(1, findIndex - 1);
		name = longName.substr(findIndex + 1, longName.size() - findIndex - 2);
		return JVMTI_ERROR_NONE;
	}
	;

	//	jvmtiError GetMethodKey(jmethodID method, char** key) {
	jvmtiError GetMethodKey(jmethodID& method, string& key) {
		jclass klass = NULL;
		char* csig;
		char* gcsig;
		char* msig;
//		(&jvmti)->GetMethodDeclaringClass(method, &klass);
		jvmti->GetMethodDeclaringClass(method, &klass);
		jvmti->GetClassSignature(klass, &csig, &gcsig);
		jvmti->GetClassSignature(klass, &csig, &gcsig);

		key = string(csig);
		return JVMTI_ERROR_NONE;
	}
	;

	jvmtiError GetPackageNameByClass(jclass& klass, string& package) {
		return JVMTI_ERROR_NONE;
	}
	;

//private:
public:
	string ReplaceStr(const string& src, const string& from, string& to) {
		string::size_type pos = 0;
		string result = string(src);
		while (pos = result.find(from, pos), pos != string::npos) {
			result.replace(pos, from.length(), to);
			pos += to.length();
		}
		return result;
	}
};
}
/* namespace jvmtipack */

#endif /* JPUTIL_HPP_ */
