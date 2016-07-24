
#include "ofApp.h"
#include "Periscope.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("PERISCOPE");
	ofSetWindowShape(1280, 768);
	ofSetWindowPosition(10, 20);
	ofSetVerticalSync(true);
	ofSetFrameRate(30);

	input.loadMovie("fingers.mov");
	
	// Classic background subtraction
	periscope.loadFromFile(ofToDataPath("BackgroundSubtract.json"));
}

//--------------------------------------------------------------
void ofApp::update(){
	input.update();
	ofImage &src = input.getInput();
	if (src.isAllocated()) {
		periscope.compute(src);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255);
	periscope.draw();
	output.send(periscope.getOutput());
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
		case 'v':
			input.selectSyphon();
			break;
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
