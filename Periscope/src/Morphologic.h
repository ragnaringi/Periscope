//
//  Created by Ragnar Hrafnkelsson on 17/10/2016.
//
//

#pragma once

#include "Component.h"

inline namespace PScope
{

class Erode : public Component
{
public:
  Erode() {
    iterations.set("Erode", 1, 0, 5);
  }
  void loadGui(ofxPanel *gui) {
    gui->add(iterations);
  };
  void compute(cv::Mat &src) {
    ofxCv::erode(src, iterations.get());
    ofxCv::copy(src, cpy);
  };
  string getTitle() {
    return "Erode";
  }
  void loadSettings(Json::Value settings) {
    iterations = settings["Settings"][iterations.getName()].asInt();
  }
  ofxJSON getSettings() {
    ofxJSON settings = Component::getSettings();
    settings["Settings"][iterations.getName()] = iterations.get();
    return settings;
  };
protected:
  ofParameter<int> iterations;
};


class Dilate : public Component
{
public:
  Dilate() {
    iterations.set("Dilate", 1, 0, 5);
  }
  void loadGui(ofxPanel *gui) {
    gui->add(iterations);
  };
  void compute(cv::Mat &src) {
    ofxCv::dilate(src, iterations.get());
    ofxCv::copy(src, cpy);
  };
  string getTitle() {
    return "Dilate";
  }
  void loadSettings(Json::Value settings) {
    iterations = settings["Settings"][iterations.getName()].asInt();
  }
  ofxJSON getSettings() {
    ofxJSON settings = Component::getSettings();
    settings["Settings"][iterations.getName()] = iterations.get();
    return settings;
  };
protected:
		ofParameter<int> iterations;
};

}
