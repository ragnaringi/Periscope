//
//  Resize.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 19/10/2016.
//
//

#pragma once

#include "Component.h"

const float MAX_SIZE = 320.f;

inline namespace PScope {

class Resize : public Component {
public:
  //!
  void loadGui( ofxPanel *gui ) override {
    gui->add(scale.set("Scale", 1, 0, 1));
  };
  //!
  void compute( cv::Mat &src ) override;
  //!
//  void compute( ofTexture &src ) override;
  //!
  string getTitle() override {
    return "Resize";
  }
  //!
  void setScale( float scale ) {
    this->scale = scale;
  }
  //!
  float getScale() {
    return scale;
  }
protected:
  ofParameter<float> scale;
};

}
