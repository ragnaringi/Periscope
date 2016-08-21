
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("PERISCOPE");
	ofSetWindowShape(1280, 768);
	ofSetWindowPosition(10, 20);
	ofSetFrameRate(60);

	input.loadMovie("fingers.mov");
//  input.selectBlackmagic();
	
	// Classic background subtraction
	periscope.loadFromFile(ofToDataPath("BackgroundSubtract.json"));
}

//--------------------------------------------------------------
void ofApp::update(){
	input.update();
	ofPixels &src = input.processed();
	if (src.isAllocated()) {
    cv::Mat mat = ofxCv::toCv(src);
    periscope.compute(mat);
	}
	output.send(periscope.getOutput());
	output.sendMain(periscope.getInput().getTexture());
  
  shapeDetector.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
  
  switch(drawingMode) {
    case 1:
      periscope.draw(); break;
    case 2:
      shapeDetector.draw(); break;
    default:
      input.draw(); break;
  }
	
  ofSetColor(ofColor::white);
	ofDrawBitmapString("1,2,3 = Change Drawing Mode, S = Save Settings to json, L = Load Settings from json, C = Select webcam, B = Select Blackmagic, V = Select syphon, M = Load movie", 10, ofGetHeight() - 100);
  ofDrawBitmapString(ofGetFrameRate(), 10, ofGetHeight() - 120);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
		case '1':
      drawingMode = 0; break;
    case '2':
      drawingMode = 1; break;
    case '3':
      drawingMode = 2; break;
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
    case 'b':
      input.selectBlackmagic();
      break;
		case 'v': {
      string server;
#ifdef __APPLE__
      server = ofSystemTextBoxDialog("Input Syphon Source", input.syphonServer());
#endif
			input.selectSyphon(server);
			break;
		}
		case 'm':
			loadMovieFile();
			break;
    case ' ':
      input.setEnabled( !input.isEnabled() );
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
