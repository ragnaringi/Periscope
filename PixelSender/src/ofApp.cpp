#include "ofApp.h"

const int PixelStripWidth = 810;

//--------------------------------------------------------------
void ofApp::setup(){
  server.setup(11999);
  
  // load a test image
  image.load("dragon.png");
  float scale = PixelStripWidth / image.getWidth(); // scale image to line up with pixel strip
  image.resize(PixelStripWidth, image.getHeight() * scale);
  
  // allocate framebuffer, 810 pixels by 1
  // this is the texture we'll be sending to pixel strip
  frameBuffer.allocate(810, 1);
}

//--------------------------------------------------------------
void ofApp::update(){
  
  // send pixel data over TCP
  server.update(2048);
  if (server.getState() == READY || server.getState() == COMPLETE) {
    
    // bind texture in framebuffer
    frameBuffer.begin();
    ofTranslate(0, -(image.getHeight() - 1)); // draw the bottom pixel row only
    image.draw(0, 0);
    frameBuffer.end();
    
    // read framebuffer texture into pixels
    frameBuffer.readToPixels(pixels);
    
    server.sendPixels(pixels.getData(), pixels.getTotalBytes());
  }
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofSetBackgroundColor(0);
  ofSetColor( ofColor::white );

  // see when client connects
  ofxTCPServer& tcpServer = server.TCP;
  for (int i = 0; i < tcpServer.getLastID(); i++){
    if ( !tcpServer.isClientConnected(i) ) continue;
    string s = "Client connected on port - " + ofToString(tcpServer.getClientPort(i));
    ofDrawBitmapString(s, 50, (i + 1) * 50);
  }
  
  // draw image
  image.draw(0, 100);
  
  // draw framebuffer that we'll be sending over TCP
  ofDrawBitmapString("These are the pixels we're sending", 10, 590);
  frameBuffer.draw(0, 600);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
