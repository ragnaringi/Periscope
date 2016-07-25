//
//  Input.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#ifndef Input_h
#define Input_h

#include <stdio.h>
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
	void update();
	void rotate(InputRotate angle);
	void crop(int x, int y, int w, int h);
	ofImage& getInput();
private:
#ifdef __APPLE__
	// Syphon
	ofxSyphonClient syphonClient;
	ofFbo syphonBuffer;
#endif
	bool isSetup;
	int x, y, w, h;
	InputRotate angle;
	unique_ptr<ofBaseVideoDraws> source;
	ofImage input;
	ofImage result;
	ofParameter<bool> enableClient, enableServer;
};
	
} /* namespace PScope */

#endif /* Input_h */
