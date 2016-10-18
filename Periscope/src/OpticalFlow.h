//
//  Flow.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 18/10/2016.
//
//

#pragma once

#include "Component.h"

inline namespace PScope {

class OpticalFlow : public Component {
  
public:
  OpticalFlow() {
    lkMaxLevel.set("lkMaxLevel", 3, 0, 8);
    lkMaxLevel.set("lkMaxLevel", 3, 0, 8);
    lkMaxFeatures.set("lkMaxFeatures", 200, 1, 1000);
    lkQualityLevel.set("lkQualityLevel", 0.01, 0.001, .02);
    lkMinDistance.set("lkMinDistance", 4, 1, 16);
    lkWinSize.set("lkWinSize", 8, 4, 64);
    fbPyrScale.set("fbPyrScale", .5, 0, .99);
    fbLevels.set("fbLevels", 4, 1, 8);
    fbIterations.set("fbIterations", 2, 1, 8);
    fbPolyN.set("fbPolyN", 7, 5, 10);
    fbPolySigma.set("fbPolySigma", 1.5, 1.1, 2);
    fbUseGaussian.set("fbUseGaussian", false);
    fbWinSize.set("winSize", 32, 4, 64);
    usefb.set("Use Farneback", false);
    
    curFlow = &lk;
  };
  void loadGui(ofxPanel *gui) {
  };
  void compute(cv::Mat &src) {
    ofxCv::copy(src, cpy);
    
    if(usefb) {
      curFlow = &fb;
      fb.setPyramidScale(fbPyrScale);
      fb.setNumLevels(fbLevels);
      fb.setWindowSize(fbWinSize);
      fb.setNumIterations(fbIterations);
      fb.setPolyN(fbPolyN);
      fb.setPolySigma(fbPolySigma);
      fb.setUseGaussian(fbUseGaussian);
    } else {
      curFlow = &lk;
      lk.setMaxFeatures(lkMaxFeatures);
      lk.setQualityLevel(lkQualityLevel);
      lk.setMinDistance(lkMinDistance);
      lk.setWindowSize(lkWinSize);
      lk.setMaxLevel(lkMaxLevel);
    }
    
    curFlow->calcOpticalFlow(cpy);
    
    // Send Osc
    float x = 0.f;
    std::vector<ofVec2f> flow = lk.getMotion();
    for (auto v : flow) {
      x += v.x;
      //      cout << v << endl;
    }
    x /= flow.size();
    cout << x << endl;
    //		ofxOscMessage m;
    //		m.setAddress("/flow");
    //		m.addFloatArg(flow.x);
    //		m.addFloatArg(flow.y);
    //		sender->sendMessage(m);
  };
  void draw(int x, int y) {
    Component::draw(x, y);
    ofPushMatrix();
    ofTranslate(x, y);
    curFlow->draw(0,0,cpy.getWidth(), cpy.getHeight());
    ofPopMatrix();
  }
  string getTitle() {
    return "Flow";
  }
protected:
  ofxCv::FlowFarneback fb;
  ofxCv::FlowPyrLK lk;
  
  ofxCv::Flow* curFlow;
  
  ofxPanel gui;
  ofParameter<float> fbPyrScale, lkQualityLevel, fbPolySigma;
  ofParameter<int> fbLevels, lkWinSize, fbIterations, fbPolyN, fbWinSize, lkMaxLevel, lkMaxFeatures, lkMinDistance;
  ofParameter<bool> fbUseGaussian, usefb;
};
  
}
