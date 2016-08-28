//
//  ObjectTracker.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 28/08/2016.
//
//

#ifndef ObjectTracker_h
#define ObjectTracker_h

// Kyle MacDonald's 'example-contours-following' from ofxCV

#include <stdio.h>
#include "ofxCv.h"

class Glow : public ofxCv::RectFollower {
protected:
  ofColor color;
  ofVec2f cur, smooth;
  float startedDying;
  ofPolyline all;
public:
  Glow()
  :startedDying(0) {
  }
  void setup(const cv::Rect& track);
  void update(const cv::Rect& track);
  void kill();
  void draw();
};

class ofxPanel;

class ObjectTracker {
public:
  void setup(ofxPanel *gui);
  void update(cv::Mat& mat);
  void draw();
  ofxCv::ContourFinder contourFinder;
  ofxCv::RectTrackerFollower<Glow> tracker;
  ofImage image;
  ofxPanel* gui;
  
  ofParameter<int> minRadius;
  ofParameter<int> maxRadius;
  ofParameter<int> threshold;
  ofParameter<int> persistence;
  ofParameter<int> maxDistance;
  ofParameter<bool> clear;
};

#endif /* ObjectTracker_h */