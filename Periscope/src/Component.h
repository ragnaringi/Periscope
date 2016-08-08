//
//  Component.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 25/07/2016.
//
//

#ifndef Component_h
#define Component_h

#include "Gui.h"
#include "ofxCv.h"
#include "ofxOsc.h"
#include "ofxJSON.h"

inline namespace PScope
{
  
class Component : public MouseAware {
public:
  Component() {
    localGui.setup();
    localGui.add(bypass.set("Bypass", false));
    localGui.add(close.set("Close", false));
    localGui.add(useRaw.set("Use Raw", false));
  }
  virtual ~Component() {};
  virtual void loadGui(ofxPanel *gui) = 0;
  virtual void loadOsc(ofxOscSender *sender) {
    this->sender = sender;
  };
  virtual string getTitle() = 0;
  virtual void compute(cv::Mat &src) = 0;
  virtual void draw(int x, int y) {
    bounds.set(x, y, cpy.getWidth(), cpy.getHeight());
    ofSetColor(ofColor::white);
    if (highlight) ofSetColor(ofColor::red);
    cpy.update();
    cpy.draw(x, y);
    ofDrawBitmapString(getTitle(), x + 10, y + 10);
    localGui.setPosition(x, y);
    localGui.draw();
  }
  bool shouldClose() { return close; };
  bool shouldUseRaw() { return useRaw; };
  bool isBypassed() { return bypass; };
  ofTexture& getTexture() {
    return cpy.getTexture();
  }
  bool selected = false;
  virtual void loadSettings(Json::Value settings) {
    bypass = settings["Settings"][bypass.getName()].asBool();
    close  = settings["Settings"][close.getName()].asBool();
    useRaw = settings["Settings"][useRaw.getName()].asBool();
  }
  virtual ofxJSON getSettings() {
    ofxJSON settings;
    settings["Title"] = getTitle();
    settings["Settings"][bypass.getName()] = bypass.get();
    settings["Settings"][close.getName()]  = close.get();
    settings["Settings"][useRaw.getName()] = useRaw.get();
    return settings;
  };
protected:
  ofParameter<bool> bypass;
  ofParameter<bool> close;
  ofParameter<bool> useRaw;
  ofImage cpy;
  ofxOscSender *sender;
  ofxPanel localGui;
};
  
}

#endif /* Component_h */
