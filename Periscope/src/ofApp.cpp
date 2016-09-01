
#include "ofApp.h"
#include "constants.h"

//--------------------------------------------------------------
void ofApp::setup(){
  
  gui.setup();
  gui.add( load.set("Load Periscope", false) );
  gui.add( save.set("Save Periscope", false) );
  gui.add( video.set("Video", false) );
  gui.add( webcam.set("Webcam", false) );
  gui.add( syphon.set("Syphon/Spout", false) );
  gui.add( hero3.set("Hero3", false) );
  gui.add( hero4.set("Hero4", true) );
  gui.add( center.set("Center", false) );
  gui.add( fitCrop.set("Fit Crop", false) );
  gui.add( fitToScreen.set("Fit to Screen", true) );
  gui.add( presentationMode.set("Presentation Mode", false) );
  gui.add( x.set("x", 320, -MAX_WIDTH, MAX_WIDTH) );
  gui.add( y.set("y", 240, -MAX_WIDTH, MAX_WIDTH) );
  gui.add( w.set("w", MAX_WIDTH, 0, MAX_WIDTH) );
  gui.add( h.set("h", MAX_HEIGHT, 0, MAX_HEIGHT) );
  gui.add( angle.set("angle", Rotate90, 0, Rotate270) );
  gui.add( debug.set("debug", true) );
  gui.add( sendOsc.set("Send OSC on Port 9997", false) );
  gui.add( zoom.set("zoom", 1.f, 1.f, 6.f) );
  gui.add( tilt.set("tilt", 0.f, 0.f, 1.f) );
  gui.add( heading.set("heading", 0.f, 0.f, 1.f) );
	
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

  input.crop(0, 0, 720, 720);
  input.rotate( Rotate90 );
  updateGui();
	
  // Classic background subtraction
  periscope.loadFromFile(ofToDataPath("Periscope.json"));
  
  objectTracker.setup( &gui );
  
  pixelSender.setup("127.0.0.1", 9001);
  
  sender.setup("127.0.0.1", SEND_PORT);
  
  receiver.setup(RECEIVE_PORT);
}

void ofApp::exit() {
#ifndef __APPLE__
	timeEndPeriod(1);
#endif  
}

//--------------------------------------------------------------
void ofApp::update(){
  processGui();
  processOscMessages();
  
  input.crop(x, y, w, h);
  input.rotate( (InputRotate)angle.get() );
  input.setZoom( zoom );
	input.update();
  
	ofPixels &src = input.processed();
	if (src.isAllocated()) {
    cv::Mat mat = ofxCv::toCv(src);
    periscope.compute(mat);
    objectTracker.update(mat);
	}
  
	output.send(periscope.getOutput());
	output.sendMain(periscope.getInput().getTexture());
  
  shapeDetector.update();
  
  pixelSender.send(src);
  
  if ( sendOsc )
    sendOscMessages();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofClear(0);
  
  switch(drawingMode) {
    case 1:
      periscope.draw(); break;
    case 2:
      objectTracker.draw(); break;
    case 3:
      shapeDetector.draw(); break;
    default:
      input.draw(fitToScreen); break;
  }
  
  if ( !debug ) return;
  ofSetColor(ofColor::white);
  ofDrawBitmapString("1,2,3 = Change Drawing Mode", 10, ofGetHeight() - 100);
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
    case '4':
      drawingMode = 3; break;
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
    case 'd':
      debug = !debug;
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

void ofApp::processGui() {
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
    updateGui();
    center = false;
  }
  else if ( fitCrop ) {
    input.fitCrop();
    updateGui();
    fitCrop = false;
  }
  
  input.presentationMode = presentationMode;
  
  // Tilting
  ofTexture &src = input.raw();
  float delta = ( src.getWidth() - h ) * 0.5;
  y = ( (src.getWidth() - h) * (1 - tilt) ) - delta;
}

//--------------------------------------------------------------
void ofApp::updateGui() {
  x = input.getCrop().getX();
  y = input.getCrop().getY();
  w = input.getCrop().getWidth();
  h = input.getCrop().getHeight();
}

void ofApp::processOscMessages() {
  while ( receiver.hasWaitingMessages() ){
    ofxOscMessage m;
    receiver.getNextMessage(m);
    
    if ( m.getAddress() == "/periscope/zoom" ){
      zoom = m.getArgAsFloat(0);
    }
    else if ( m.getAddress() == "/periscope/heading" ){
      heading = m.getArgAsFloat( 0 ) / 360.f; // Normalise to 0 - 1
      printf("heading: %f\n", heading.get());
    }
  }
}

//--------------------------------------------------------------
void ofApp::sendOscMessages() {
  // Send Orientation
  {
  ofxOscMessage m;
  m.setAddress("/periscope/orientation");
  m.addFloatArg(heading); // TODO: Receive from Periscope
  m.addFloatArg(tilt);
  sender.sendMessage(m);
  }
  // Send Zoom
  {
  ofxOscMessage m;
  m.setAddress("/periscope/zoom");
  m.addFloatArg(zoom);
  sender.sendMessage(m);
  }
  
  // Send Number Of Objects
  {
    const vector<cv::Rect>& boundingRects = objectTracker.getBoundingRects();
    ofxOscMessage m;
    m.setAddress("/periscope/numObjects");
    m.addIntArg(boundingRects.size());
    sender.sendMessage(m);
  }
  
  // Send Number Of People
}
