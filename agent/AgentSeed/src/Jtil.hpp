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
#include <sstream>

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

	jvmtiError GetClassName(jclass& klass, string& package,
			string& simpleName) {
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
			simpleName = longName.substr(1, longName.size() - 2);
			return JVMTI_ERROR_NONE;
		}
		package = longName.substr(1, findIndex - 1);
		simpleName = longName.substr(findIndex + 1,
				longName.size() - findIndex - 2);
		return JVMTI_ERROR_NONE;
	}

	jvmtiError GetClassName(jclass& klass, string& package, string& simpleName,
			string& canonicalName) {
		jvmtiError error = GetClassName(klass, package, simpleName);
		CHECK_RESULT("GetClassName", error);
		canonicalName = string();
		if (package.size() != 0) {
			canonicalName.append(package);
			canonicalName.append("/");
		}
		canonicalName.append(simpleName);
		return JVMTI_ERROR_NONE;
	}

	//	jvmtiError GetMethodKey(jmethodID method, char** key) {
	jvmtiError GetMethodKey(jmethodID& method, string& key) {
		jvmtiError error;
		jclass klass;
		char *csig, *gcsig;
		char *mname, *msig, *mgsig;
		stringstream ss;

		error = jvmti->GetMethodDeclaringClass(method, &klass);
		CHECK_RESULT("GetMethodKey0", error);
		error = jvmti->GetClassSignature(klass, &csig, &gcsig);
		CHECK_RESULT("GetMethodKey1", error);
		error = jvmti->GetMethodName(method, &mname, &msig, &mgsig);
		CHECK_RESULT("GetMethodKey2", error);
		ss << csig << "#" << mname << msig;
		key = ss.str();
		return JVMTI_ERROR_NONE;
	}

	jvmtiError GetPackageNameByClass(jclass& klass, string& package) {
		return JVMTI_ERROR_NONE;
	}

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
