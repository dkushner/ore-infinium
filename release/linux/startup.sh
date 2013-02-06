#! /bin/sh

BASEPATH=$(dirname $(readlink -f $0))
if [ -z "$LD_LIBRARY_PATH" ]
  NEW_LD_L_P=$BASEPATH
else
  NEW_LD_L_P=$BASEPATH:$LD_LIBRARY_PATH
fi

LD_LIBRARY_PATH=$NEW_LD_L_P $BASEPATH/mygame

