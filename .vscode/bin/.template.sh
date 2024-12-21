#!/usr/bin/env bash
### templates start ------------------------------------
### ------------bash Grammer memo-----------------------
# Arguments
# $0    this filename
# $#	len(argv[1:])
# $N    argv[N]
# ${N}  argv[N]
# "$@"	argv[1:] (Enabled double quote separater) <- Recommend
# $@    argv[1:] (Ignored double quote separater)
# $*	argv[1:] (Ignored double quote separater)
# "$*"	argv[1:] (Ignored double quote separater)
# ${@: x: n} argv[x:n]
# ${@: x: $# + 1} argc[x:]
#
#
## if , elif, else =====================================
#
#if [[ EXPRESSION1 ]];then
#   ...
#elif [[ EXPRESSION2 ]]
#   ...
#else
#   ...
#fi

## compare =============================================
#
#NUM1 -eq NUM2	(NUM1 == NUM2)
#NUM1 -ne NUM2	(NUM1 != NUM2)
#NUM1 -ge NUM2	(NUM1 >= NUM2)
#NUM1 -gt NUM2	(NUM1 >  NUM2)
#NUM1 -le NUM2	(NUM1 <= NUM2)
#NUM1 -lt NUM2	(NUM1 <  NUM2)
#NUM1 == NUM2
#NUM1 != NUM2
#NUM1 >  NUM2
#NUM1 <  NUM2
#-n STR	len >= 1
#-z STR	len == 0
#STR1 == STR2
#STR1 != STR2
#STR1 >  STR2
#STR1 <  STR2
#STR1 =~ STR2 (regex but bash3.0 over)

## AND , OR , NOT =======================================
#
#EX1 && EX2	(AND)
#EX1 || EX2	(OR)
#! EX1	(NOT)

## validation files =====================================
#
### https://qiita.com/nii_yan/items/73f4caacdc2ca6f45135
#
### -----------------------------------------------------

function raise() {
    echo "error: $*" >&2
    exit 1
}

if [[ $# < N ]]; then
    echo "This Script is xxx" >&2
    MYNAME=`basename $0 | sed -E "s/\.sh$//g"`
    echo "Usage: ${MYNAME} <arg1 desc> <arg2 desc> [arg3 desc]" >&2
    exit 1
fi


set -eu
RETCODE=0
function catch {
  RETCODE=1
}
trap catch ERR

### templates end --------------------------------------


exit $RETCODE
