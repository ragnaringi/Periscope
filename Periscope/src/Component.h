//
//  Component.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 25/07/2016.
//
//

#pragma once

#include "Gui.h"
#include "ofxCv.h"
#include "ofxJSON.h"
#include "OscSender.h"

inline namespace PScope {
  
class Component : public MouseAware, public OscSender {
public:
  //!
  Component() {
    localGui.setup();
    localGui.add( bypass.set("Bypass", false) );
    localGui.add( close.set("Close", false) );
    localGui.add( useRaw.set("Use Raw", false) );
  }
  //!
  virtual ~Component() {};
  //!
  virtual void loadGui(ofxPanel *gui) = 0;
  
  //!
  virtual string getTitle() = 0;
  //!
  virtual void compute(cv::Mat &src) = 0;
  //!
  virtual void draw(int x, int y) {
    bounds.set(x, y, cpy.getWidth(), cpy.getHeight());
    ofSetColor(ofColor::white);
    if (highlight) ofSetColor(ofColor::red);
    ofTexture& tex = getTexture();
    tex.draw(x, y);
    ofDrawBitmapString(getTitle(), x + 10, y + 10);
    localGui.setPosition(x, y);
    localGui.draw();
  }
  //!
  bool selected = false;
  //!
  bool shouldClose() { return close; };
  //!
  bool shouldUseRaw() { return useRaw; };
  //!
  bool isBypassed() { return bypass; };
  ofTexture& getTexture() {
    cpy.update();
    return cpy.getTexture();
  }
  
  //!
  virtual void loadSettings( Json::Value settings ) {
    bypass = settings["Settings"][bypass.getName()].asBool();
    close  = settings["Settings"][close.getName()].asBool();
    useRaw = settings["Settings"][useRaw.getName()].asBool();
  }
  
  //!
  virtual ofxJSON getSettings() {
    ofxJSON settings;
    settings["Title"] = getTitle();
    settings["Settings"][bypass.getName()] = bypass.get();
    settings["Settings"][close.getName()]  = close.get();
    settings["Settings"][useRaw.getName()] = useRaw.get();
    return settings;
  };
  
protected:
  ofImage cpy;
  ofxPanel localGui;
  std::string title;
  
private:
  ofParameter<bool> bypass;
  ofParameter<bool> close;
  ofParameter<bool> useRaw;
};
  
}
