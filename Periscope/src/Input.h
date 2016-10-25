//
//  Input.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#pragma once

#include "Resize.h"
#include "ofxBlackmagic.h"
#include "ofxGui.h"
#ifdef __APPLE__
#include "ofxSyphon.h"
#else
#include "ofxSpout2Receiver.h"
#endif

inline namespace PScope
{

class Input {
public:
	Input();
	void loadMovie(std::string title);
	void selectWebCam();
  void selectBlackmagic(BMDDisplayMode mode);
	void selectSyphon(std::string server);
//	void rotate(InputRotate angle);
//  ofRectangle getCrop();
//	void crop(int x, int y, int w, int h);
//  void centerCrop();
//  void fitCrop();
//  void setCenter(int x, int y);
//  void setZoom(float zoom);
	void update();
//	void draw(bool fitToSize);
  bool& isEnabled() { return enabled; }
  void setEnabled(bool enable) { enabled = enable; }
	ofTexture& getTexture();
//	ofPixels& processed();
#ifdef __APPLE__
  string& syphonServer() {
    return syphonClient.getApplicationName();
  }
#endif
  bool presentationMode = false;
private:
  std::unique_ptr<ofxBlackmagic::Input> input;
#ifdef __APPLE__
	ofxSyphonClient syphonClient;
#else
	ofxSpout2::Receiver spoutReceiver;
#endif
	bool isSetup;
  bool enabled;
//  float zoom;
//	int angle;
	unique_ptr<ofBaseVideoDraws> source;
	ofParameter<bool> enableClient, enableServer;
	void updateGui();
//  void updateTextureIfNeeded();
//  bool textureNeedsUpdate;
};
	
} /* namespace PScope */

