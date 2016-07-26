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
#ifdef __APPLE__
#include "ofxSyphon.h"
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
	void selectSyphon(std::string server);
	void rotate(InputRotate angle);
	void crop(int x, int y, int w, int h);
	void update();
	void draw();
	ofImage& raw();
	ofImage& processed();
private:
#ifdef __APPLE__
	ofxSyphonClient syphonClient;
	ofFbo syphonBuffer;
#endif
	bool isSetup;
	ofParameter<int> x, y, w, h, angle;
	unique_ptr<ofBaseVideoDraws> source;
	ofImage input;
	ofImage result;
	ofxPanel gui;
	ofParameter<bool> enableClient, enableServer;
	void updateGui();
};
	
} /* namespace PScope */

#endif /* Input_h */
