//
//  ObjectTracker.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 28/08/2016.
//
//

#include "ofxGui.h"
#include "ofGraphics.h"
#include "ObjectTracker.h"

const float dyingTime = 1;

void Glow::setup(const cv::Rect& track) {
  color.setHsb(ofRandom(0, 255), 255, 255);
  cur = ofxCv::toOf(track).getCenter();
  smooth = cur;
}

void Glow::update(const cv::Rect& track) {
  cur = ofxCv::toOf(track).getCenter();
  smooth.interpolate(cur, .5);
  all.addVertex(smooth);
}

void Glow::kill() {
  float curTime = ofGetElapsedTimef();
  if(startedDying == 0) {
    startedDying = curTime;
  } else if(curTime - startedDying > dyingTime) {
    dead = true;
  }
}

void Glow::draw() {
  ofPushStyle();
  float size = 16;
  ofSetColor(255);
  if(startedDying) {
    ofSetColor(ofColor::red);
    size = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, size, 0, true);
  }
  ofNoFill();
  ofDrawCircle(cur, size);
  ofSetColor(color);
  all.draw();
  ofSetColor(255);
  ofDrawBitmapString(ofToString(label), cur);
  ofPopStyle();
}

extern void center(const ofRectangle &rect, int angle);
extern void applyRotation(const ofTexture &image, int angle);
extern void applyRotation(const ofRectangle &rect, int angle);
extern void center(const ofRectangle& rect, ofFbo& container, int angle);
extern void center(const ofTexture& texture, ofFbo& container, int angle);

void ObjectTracker::setup(ofxPanel *gui) {
  gui->add( minRadius.set("Min Radius", 1, 0, 200) );
  gui->add( maxRadius.set("Max Radius", 100, 0, 1080) );
  gui->add( threshold.set("Threshold", 15, 0, 255) );
  gui->add( persistence.set("Persistence", 15, 0, 255) );
  gui->add( maxDistance.set("Max Distance", 100, 0, 1080) );
}

void ObjectTracker::update(cv::Mat &mat) {
  contourFinder.setMinAreaRadius( minRadius );
  contourFinder.setMaxAreaRadius( maxRadius );
  contourFinder.setThreshold( threshold );
  tracker.setPersistence( persistence );
  tracker.setMaximumDistance( maxDistance );
  
  contourFinder.findContours(mat);
  tracker.track(contourFinder.getBoundingRects());
  ofxCv::copy(mat, image);
}

void ObjectTracker::draw() {
  ofSetColor(255);
  ofPushMatrix();
  ofRectangle rect(0, 0, image.getWidth(), image.getHeight());
  center(rect, 0);
  applyRotation(rect, 0);
  image.update();
  image.draw(0, 0);
  vector<Glow>& followers = tracker.getFollowers();
  for(int i = 0; i < followers.size(); i++) {
    followers[i].draw();
  }
  ofPopMatrix();
}

const vector<cv::Rect>& ObjectTracker::getBoundingRects() {
  return contourFinder.getBoundingRects();
}
