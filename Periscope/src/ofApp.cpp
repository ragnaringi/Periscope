
#include "ofApp.h"
#include "Periscope.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("PERISCOPE");
	ofSetWindowShape(1280, 768);
	ofSetWindowPosition(10, 20);
	ofSetFrameRate(60);

	input.loadMovie("fingers.mov");
	
	// Classic background subtraction
	periscope.loadFromFile(ofToDataPath("BackgroundSubtract.json"));
}

//--------------------------------------------------------------
void ofApp::update(){
	input.update();
	ofImage &src = input.processed();
	if (src.isAllocated()) {
		periscope.compute(src);
	}
	output.send(periscope.getOutput());
	output.sendMain(src.getTexture());
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
	
	if (!periscope.getDebug()) {
		input.draw();
		return;
	}

	periscope.draw();
	ofSetColor(ofColor::white);
	ofDrawBitmapString("S = Save Settings to json, L = Load Settings from json, C = Select webcam, V = Select syphon, M = Load movie", 10, ofGetHeight() - 100);
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
		case 'l':
			loadPeriscope();
			break;
		case 's':
			savePeriscope();
			break;
		case 'c':
			input.selectWebCam();
			break;
		case 'v': {
			string server = ofSystemTextBoxDialog("Input Syphon Source", "Simple Server");
			input.selectSyphon(server);
			break;
		}
		case 'm':
			loadMovieFile();
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

//--------------------------------------------------------------
void ofApp::loadPeriscope(){
	ofFileDialogResult result = ofSystemLoadDialog();
	cout << result.getPath() << endl;
	periscope.loadFromFile(result.getPath());
}

//--------------------------------------------------------------
void ofApp::savePeriscope(){
	ofFileDialogResult result = ofSystemSaveDialog("Periscope.json", "Save your Periscope");
	cout << result.getPath() << endl;
	periscope.saveToFile(result.getPath());
}

//--------------------------------------------------------------
void ofApp::loadMovieFile() {
	ofFileDialogResult result = ofSystemLoadDialog();
	cout << result.getPath() << endl;
	input.loadMovie(result.getPath());
}
