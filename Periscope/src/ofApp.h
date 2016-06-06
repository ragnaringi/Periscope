
#pragma once

#include "ofMain.h"
#include "Periscope.h"

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
    
//#ifdef _USE_LIVE_VIDEO
//		ofVideoGrabber cam;
//#else
		ofVideoPlayer cam;
//#endif
		ofPixels previous;
		ofImage diff;
		
		// a scalar is like an ofVec4f but normally used for storing color information
		cv::Scalar diffMean;
		cv::Mat columnMean;

		// Flow
		ofxCv::FlowFarneback fb;
		ofxCv::FlowPyrLK lk;
	
		ofxCv::Flow* curFlow;
	
		Periscope periscope;
	
//		ofParameter<float> fbPyrScale, lkQualityLevel, fbPolySigma;
//		ofParameter<int> fbLevels, lkWinSize, fbIterations, fbPolyN, fbWinSize, lkMaxLevel, lkMaxFeatures, lkMinDistance;
//		ofParameter<bool> fbUseGaussian, usefb;
//		// Flow Keypoints
//		ofVideoGrabber grabber;
//		cv::Mat grabberGray;
//		ofxCv::FlowPyrLK flow;
//		ofVec2f p1;
//		ofRectangle rect;
};
