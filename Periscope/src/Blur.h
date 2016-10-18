//
//  Blur.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 18/10/2016.
//
//

#pragma once

#include "Component.h"

inline namespace PScope {

class Blur : public Component {
public:
  Blur() {
    blurAmt.set("Blur", 5, 0, 25);
  }
  void loadGui(ofxPanel *gui) {
    gui->add(blurAmt);
  };
  void compute(cv::Mat &src) {
    if (blurAmt > 0) {;
      ofxCv::blur(src, blurAmt);
    }
    ofxCv::copy(src, cpy);
  };
  string getTitle() {
    return "Blur";
  }
  void loadSettings(Json::Value settings) {
    blurAmt = settings["Settings"][blurAmt.getName()].asInt();
  }
  ofxJSON getSettings() {
    ofxJSON settings = Component::getSettings();
    settings["Settings"][blurAmt.getName()] = blurAmt.get();
    return settings;
  };
protected:
  ofParameter<int> blurAmt;
};

}
