
#pragma once

#include "Input.h"
#include "Output.h"
#include "Periscope.h"
#include "ShapeDetector.h"
#include "PixelSender.h"
#include "ofMain.h"

#ifndef __APPLE__
#include <mmsystem.h>
#endif

class ofApp : public ofBaseApp{

	public:
		void setup();
		void exit();
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
	
	private:
  
    void processGui();
    void updateGui();
    void sendOscMessages();
    void processOscMessages();
  
    int drawingMode;
	
		PScope::Input input;
		PScope::Output output;
		PScope::Periscope periscope;
    PScope::ShapeDetector shapeDetector;
    PixelSender pixelSender;
	
		void loadPeriscope();
		void savePeriscope();
		void loadMovieFile();
    void selectSyphonInput();
  
    ofxOscSender sender;
    ofxOscReceiver receiver;
  
    ofxPanel gui;
    ofParameter<bool> load;
    ofParameter<bool> save;
    ofParameter<bool> video;
    ofParameter<bool> webcam;
    ofParameter<bool> syphon;
    ofParameter<bool> hero3;
    ofParameter<bool> hero4;
    ofParameter<bool> center;
    ofParameter<bool> fitToSize;
    ofParameter<bool> debug;
    ofParameter<bool> sendOsc;
    ofParameter<int> x, y, w, h, angle;
  
    // Periscope parameters
    ofParameter<float> zoom;
    ofParameter<float> heading;
    int numberOfObjects;
  
};
