
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  
  gui.setup();
  gui.add( load.set("Load Periscope", false) );
  gui.add( save.set("Save Periscope", false) );
  gui.add( video.set("Video", false) );
  gui.add( webcam.set("Webcam", false) );
  gui.add( syphon.set("Syphon/Spout", false) );
  gui.add( hero3.set("Hero3", false) );
  gui.add( hero4.set("Hero4", false) );
  gui.add( center.set("Center", false) );
  gui.add( fitToSize.set("Fit to Size", false) );
	
  ofSetWindowTitle("PERISCOPE");
  ofSetWindowShape(1280, 768);
  ofSetWindowPosition(10, 20);
  ofSetVerticalSync(true);
  ofSetFrameRate(60);
#ifndef __APPLE__ 
  // Attempt to get max framerate on Windows
  // https://forum.openframeworks.cc/t/emptyexample-running-at-50fps-in-release-mode/13731/7
  timeBeginPeriod(1);
#endif

  input.loadMovie("fingers.mov");
//  input.selectBlackmagic();
  input.crop(0, 0, 720, 720);
	
  // Classic background subtraction
  periscope.loadFromFile(ofToDataPath("BackgroundSubtract.json"));
}

void ofApp::exit() {
#ifndef __APPLE__
	timeEndPeriod(1);
#endif  
}

//--------------------------------------------------------------
void ofApp::update(){
  if ( load ) {
    loadPeriscope();
    load = false;
  }
  else if ( save ) {
    savePeriscope();
    save = false;
  }
  else if ( video ) {
    loadMovieFile();
    video = false;
  }
  else if ( webcam ) {
    input.selectWebCam();
    webcam = false;
  }
  else if ( syphon ) {
    selectSyphonInput();
    syphon = false;
  }
  else if ( hero3 ) {
    input.selectBlackmagic(bmdModeHD720p50);
    hero3 = false;
  }
  else if ( hero4 ) {
    input.selectBlackmagic(bmdModeHD720p5994);
    hero4 = false;
  }
  else if ( center ) {
    input.centerCrop();
    center = false;
  }
  else if ( fitToSize ) {
    input.fitCrop();
    fitToSize = false;
  }
  
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
	ofDrawBitmapString("1,2,3 = Change Drawing Mode, S = Save Settings to json, L = Load Settings from json", 10, ofGetHeight() - 100);
  ofDrawBitmapString(ofGetFrameRate(), 10, ofGetHeight() - 120);
  
  gui.draw();
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

//--------------------------------------------------------------
void ofApp::selectSyphonInput() {
  string server;
#ifdef __APPLE__
  server = ofSystemTextBoxDialog("Input Syphon Source", input.syphonServer());
#endif
  input.selectSyphon(server);
}
