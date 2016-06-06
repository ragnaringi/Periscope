#!/usr/bin/env bash

git clone https://github.com/kylemcdonald/ofxCv.git openFrameworks/addons/ofxCv
git clone https://github.com/kylemcdonald/ofxCcv.git openFrameworks/addons/ofxCcv
git clone https://github.com/braitsch/ofxDatGui.git openFrameworks/addons/ofxDatGui
mkdir Periscope/bin/data/ofxbraitsch
cp -r openFrameworks/addons/ofxDatGui/ofxbraitsch/* Periscope/bin/data/ofxbraitsch
cd openFrameworks/addons/ofxCcv
./setup.sh