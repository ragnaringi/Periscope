//
//  Input.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#ifndef Input_h
#define Input_h

#include "ofxGui.h"
#include "ofxBlackmagic.h"
#ifdef __APPLE__
#include "ofxSyphon.h"
#else
#include "ofxSpout2Receiver.h"
#endif

inline namespace PScope
{
	
enum InputRotate {
	RotateNone,
	Rotate90,
	Rotate180,
	Rotate270
};

class Input {
public:
	Input();
	void loadMovie(std::string title);
	void selectWebCam();
  void selectBlackmagic();
	void selectSyphon(std::string server);
	void rotate(InputRotate angle);
	void crop(int x, int y, int w, int h);
	void update();
	void draw();
  bool& isEnabled() { return enabled; }
  void setEnabled(bool enable) { enabled = enable; }
	ofTexture& raw();
	ofPixels& processed();
#ifdef __APPLE__
  string& syphonServer() {
    return syphonClient.getApplicationName();
  }
#endif
private:
	ofFbo frameBuffer;
  std::unique_ptr<ofxBlackmagic::Input> input;
#ifdef __APPLE__
	ofxSyphonClient syphonClient;
#else
	ofxSpout2::Receiver spoutReceiver;
#endif
	bool isSetup;
  bool enabled;
	ofParameter<int> x, y, w, h, angle;
	unique_ptr<ofBaseVideoDraws> source;
	ofPixels result;
	ofxPanel gui;
	ofParameter<bool> enableClient, enableServer;
	void updateGui();
  void updateTextureIfNeeded();
  bool textureNeedsUpdate;
};
	
} /* namespace PScope */

#endif /* Input_h */
