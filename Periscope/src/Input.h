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

class Input {
public:
	Input();
	void loadMovie(std::string title);
	void selectWebCam();
	void selectSyphon();
	void update();
	ofImage& getInput();
private:
#ifdef __APPLE__
	// Syphon
	ofxSyphonClient syphonClient;
	ofFbo syphonBuffer;
#endif
	unique_ptr<ofBaseVideoDraws> source;
	ofImage input;
	ofParameter<bool> enableClient, enableServer;
};
	
} /* namespace PScope */

#endif /* Input_h */
