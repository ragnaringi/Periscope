
#include "ofApp.h"
#include "Periscope.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("PERISCOPE");
	ofSetWindowShape(1152, 768);
	ofSetWindowPosition(40, 20);
	ofSetVerticalSync(true);
	ofSetFrameRate(30);

	periscope.loadMovie("fingers.mov");

	Resize *resize = new Resize();
	GrayScale *grayScale = new GrayScale();
	Difference *d = new Difference();
	Threshold *t = new Threshold();
	Blur *blur = new Blur();
	Contours *c = new Contours();
	periscope.addComponent(resize);
	periscope.addComponent(grayScale);
	periscope.addComponent(d);
	periscope.addComponent(t);
	periscope.addComponent(blur);
	periscope.addComponent(c);
}

//--------------------------------------------------------------
void ofApp::update(){

	periscope.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
	ofSetColor(255);
	periscope.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
		case ' ':
			cam.firstFrame();
			break;
		case 'f':
			ofToggleFullscreen();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
