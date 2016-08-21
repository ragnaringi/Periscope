#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  client.setup("127.0.0.1", 11999);
  
  tex.allocate(810, 1, GL_RGB);
  pixelsLoaded = false;
}

//--------------------------------------------------------------
void ofApp::update(){
  client.update(2048);
  
  if( client.getState() == COMPLETE ){
    tex.loadData(client.pixels, 810, 1, GL_RGB);
    pixelsLoaded = true;
  }
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofSetBackgroundColor( 0 );
  ofSetColor( 255 );
  
  string statusStr =  "status: " + client.getStateStr();
  statusStr += " -- loaded "+ofToString(client.getPctLoaded(), 2)+"%";
  ofDrawBitmapString(statusStr, 10, 20);
  
  if( pixelsLoaded ){
    tex.draw(0, 125);
  }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  client.reset();
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
