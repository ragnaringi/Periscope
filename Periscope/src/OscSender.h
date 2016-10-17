//
//  OscSender.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 17/10/2016.
//
//

#pragma once

#include "ofxOsc.h"

inline namespace PScope {

class OscSender {

public:
  //!
  virtual void loadOsc(ofxOscSender *sender) {
    this->sender = sender;
  };

protected:
  ofxOscSender *sender;
  
};
  
}
