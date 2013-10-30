#!/bin/sh
#begin function
function help(){
    echo "usage : `basename $0` AgentName"
    echo ""
    echo "    ex# `basename $0` TraceAgent"
    echo ""
}
#end function
if [ $# -lt 1 ];then
    help
    exit;
fi
#AGENT_NAME=MethodTrace
AGENT_NAME=$1
SHELL_DIR=$(cd $(dirname $0); pwd)
TMP_DIR=${SHELL_DIR}/tmp
CWD=`pwd`
JAVA_CLASS=jv.sample.Hello
JAVA_CLASS=jv.sample.Heyhey
JAVA_CLASSPATH=${SHELL_DIR}/classes
JAVA_ARGS=
AGENT_OPTION="=jvmtioptions..."
AGENT_LIB=${SHELL_DIR}/../agent/${AGENT_NAME}/Debug/lib${AGENT_NAME}.dylib

cp ${AGENT_LIB} ${TMP_DIR}/
cd ${TMP_DIR}

java -agentlib:${AGENT_NAME}${AGENT_OPTION} -cp ${JAVA_CLASSPATH}:${CLASSPATH} ${JAVA_CLASS} ${JAVA_ARGS}

cd ${CWD}

