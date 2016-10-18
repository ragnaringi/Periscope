//
//  FeatureDetect.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 18/10/2016.
//
//

#pragma once

#include "Component.h"
#include "Periscope.h" // TODO: Remove

inline namespace PScope {
  
//=================================
#pragma mark - Canny Edge Detection
//=================================
  
class Canny : public Component {
public:
  //!
  Canny() {
    thresh1.set("Threshold 1", 1, 0, 200);
    thresh2.set("Threshold 2", 30, 0, 200);
  }
  //!
  void loadGui(ofxPanel *gui) {
    gui->add(thresh1);
    gui->add(thresh2);
  };
  //!
  void compute(cv::Mat &src) {
    ofxCv::copyGray(src, cpy); // grayscale 8-bit input and output
    ofxCv::Canny(cpy, cpy, thresh1, thresh2);
    ofxCv::copy(cpy, src);
  };
  //!
  string getTitle() {
    return "Canny";
  }
  //!
  void loadSettings(Json::Value settings) {
    thresh1 = settings["Settings"][thresh1.getName()].asInt();
    thresh2 = settings["Settings"][thresh2.getName()].asInt();
  }
  //!
  ofxJSON getSettings() {
    ofxJSON settings = Component::getSettings();
    settings["Settings"][thresh1.getName()] = thresh2.get();
    settings["Settings"][thresh2.getName()] = thresh2.get();
    return settings;
  };
protected:
  ofParameter<int> thresh1;
  ofParameter<int> thresh2;
};
  
//=================================
#pragma mark - HoughLines
//=================================
// Provide a binary image by running through Canny Edge detection or Threshold before HoughLines
// http://stackoverflow.com/questions/9310543/whats-the-use-of-canny-before-houghlines-opencv
// Default values are obtained from ofxCv::autorotate function in Helpers.h
class Hough : public Component {
public:
  //!
  Hough() {}
  ~Hough() {}
  //!
  void loadGui(ofxPanel *gui) override {}
  //!
  void compute(cv::Mat &src) override {
    ofImage& raw = Periscope::getInput();
    cv::Mat srcMat = ofxCv::toCv(raw);
    cv::Mat& threshMat = src;
    lines.clear();
    double distanceResolution = 1;
    double angleResolution = CV_PI / 180;
    int voteThreshold = 10;
    double minLineLength = (srcMat.rows + srcMat.cols) / 8;
    double maxLineGap = 3;
    HoughLinesP(threshMat, lines, distanceResolution, angleResolution, voteThreshold, minLineLength, maxLineGap);
    
    ofxCv::copy(src, cpy);
  }
  //!
  void draw( int x, int y ) override {
    Component::draw(x, y);
    ofPushMatrix();
    ofTranslate(x, y);
    // draw lines
    ofSetColor(ofColor::red);
    for( size_t i = 0; i < lines.size(); i++ ) {
      ofDrawLine(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
    }
    ofPopMatrix();
  }
  //!
  std::string getTitle() override {
    return "Hough Lines";
  }
protected:
  std::vector<cv::Vec4i> lines;
};
  
}
