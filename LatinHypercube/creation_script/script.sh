#!/bin/bash

if [ $1 = "-h" ]
then
	echo "This script automatise the creation and the use of pbrt"
	echo "usage : $0 -pbrt LatinHypercubeExec SceneInputFile NbCamSample NbLightSample"
	echo "        $0 -data PBRTExec lowRange HighRange"
	echo
	echo "LowRange and HighRange indicate the range of scene file to use"
fi

if [ $1 = "-pbrt" ]
then
	if [ ! -d ./scene_file ]
	then
		mkdir ./scene_file
	fi
	cd ./scene_file

	#Creation des fichier pbrt
	"../$2" ../$3 $4 $5
fi

if [ $1 = "-data" ]
then
	if [ ! -d ./scene_file ]
	then
		echo "You must use the -pbrt option before the -data"
	else
		if [ ! -d ./data_file ]
		then
			mkdir ./data_file
		fi
		cd ./data_file

		for i in $(seq $3 $4)
		do
			"../$2" ../scene_file/*$i*
		done
	fi
fi