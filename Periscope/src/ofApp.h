#pragma once

#include "ofMain.h"
#include "ofxCv.h"

//#define _USE_LIVE_VIDEO		// uncomment this to use a live camera
// otherwise, we'll use a movie file

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
#ifdef _USE_LIVE_VIDEO
		ofVideoGrabber cam;
#else
		ofVideoPlayer cam;
#endif
		ofPixels previous;
		ofImage diff;
		
		// a scalar is like an ofVec4f but normally used for storing color information
		cv::Scalar diffMean;
		cv::Mat columnMean;
	
		// Countour tracking
		ofxCv::ContourFinder contourFinder;
		bool showLabels;
		bool analyseDiff;
};
