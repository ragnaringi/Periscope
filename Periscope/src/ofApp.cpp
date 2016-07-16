
#include "ofApp.h"
#include "Periscope.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("PERISCOPE");
	ofSetWindowShape(1280, 768);
	ofSetWindowPosition(10, 20);
	ofSetVerticalSync(true);
	ofSetFrameRate(30);

	periscope.loadMovie("fingers.mov");

	// Classic background subtraction
	periscope.addComponent(new Resize());
	periscope.addComponent(new Colours());
	periscope.addComponent(new GrayScale());
	periscope.addComponent(new OpticalFlow());
	periscope.addComponent(new Difference());
	periscope.addComponent(new Blur());
	periscope.addComponent(new Threshold());
	periscope.addComponent(new Contours());
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
			periscope.setDebug( !periscope.getDebug() );
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
	periscope.mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	periscope.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	periscope.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	periscope.mouseReleased(x, y, button);
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
