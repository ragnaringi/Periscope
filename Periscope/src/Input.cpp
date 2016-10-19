//
//  Input.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#include "constants.h"
#include "Input.h"

ofTexture emptyTexture = ofTexture();

//--------------------------------------------------------------
Input::Input() : isSetup(false), enabled(true) {
#ifdef __APPLE__
  // Syphon setup
  syphonClient.setup(); //using Syphon app Simple Server, found at http://syphon.v002.info/
  syphonClient.set("","Simple Server");
#else
  // Spout setup
  spoutReceiver.setup();
#endif
}

//--------------------------------------------------------------
void Input::loadMovie(std::string title) {
  input = nullptr;
  enableClient = false;
  unique_ptr<ofVideoPlayer> player(new ofVideoPlayer);
  if ( !player->load(title) ) {
    cout << "Error loading movie: " << title << endl;
  }
  player->play();
  player->setLoopState(OF_LOOP_NORMAL);
  source = move(player);
  isSetup = false;
}

//--------------------------------------------------------------
void Input::selectWebCam() {
  input = nullptr;
  enableClient = false;
  unique_ptr<ofVideoGrabber> cam(new ofVideoGrabber);
  cam->setup(640, 480);
  source = move(cam);
  isSetup = false;
}

//--------------------------------------------------------------
void Input::selectBlackmagic(BMDDisplayMode mode) {
  enableClient = false;
  source = nullptr;
  auto deviceList = ofxBlackmagic::Iterator::getDeviceList();
  if ( deviceList.empty() ) {
    cout << "No Blackmagic device found" << endl;
    return;
  }
  std::unique_ptr<ofxBlackmagic::Input> cam( new ofxBlackmagic::Input() );
  cam->startCapture(deviceList.front(), mode);
  input = move( cam );
  isSetup = false;
}

//--------------------------------------------------------------
void Input::selectSyphon(std::string server) {
#ifdef __APPLE__
  syphonClient.set("", server);
#endif
  source = nullptr;
  enableClient = true;
  isSetup = false;
}

//--------------------------------------------------------------
void Input::update() {
  if ( enabled && !enableClient ) {
    if ( source != nullptr ) {
      source->update();
      if( !source->isFrameNew() ) return;
    }
    if (input != nullptr) {
      input->update();
      if ( !input->isFrameNew() ) return;
    }
  }
  else {
#ifndef __APPLE__
    spoutReceiver.updateTexture();
#endif
  }
  updateGui();
}

//--------------------------------------------------------------
//void Input::draw(bool fitToSize) {
////  updateTextureIfNeeded();
//  
//  ofPushStyle();
//  
//  ofClear(0.f);
//  ofSetColor(ofColor::white);
//  
//  ofTexture& input = getTexture();
//  if ( !presentationMode ) {
//  // Center images using original as anchor
//
//    ofPushMatrix();
//    if ( fitToSize ) {
//      float scale = 1.f;
////      if ( angle % 2 == 0 ) {
////        // TODO:(Ragnar)
////        scale = 1.f;
////      }
////      else {
////        if ( input.getWidth() > ofGetHeight() ) {
////          scale = ofGetHeight() / input.getWidth();
////          ofTranslate((input.getWidth() - input.getHeight()) * 0.5, input.getWidth() / 8);
////        }
////      }
//      ofScale( scale, scale );
//    }
//    
//    {
//      ofPushMatrix();
////      resize->compute( input );
//      input.draw(0, 0);
//      ofPopMatrix(); /* Center images */
//    }
//    
//    // Draw bounding box for crop
//    {
//#warning TODO 
//      // Move to app class
//      ofPushMatrix();
////      ofRectangle rect = getCrop();
////      center(rect, 0);
////      ofNoFill();
////      ofSetColor(ofColor::red);
////      ofDrawRectangle(rect);
//      ofPopMatrix();
//    }
//    
//    ofPopMatrix();
//  }
//  else {
//    float scale = (float)ofGetWidth() / (float)input.getWidth();
//    input.draw(0, 0, ofGetWidth(), input.getHeight() * scale);
//  }
//  
//  ofPopStyle();
//}

//--------------------------------------------------------------
//void Input::rotate(InputRotate angle_) {
//  angle = angle_;
//}

//--------------------------------------------------------------
ofTexture& Input::getTexture() {
  if ( enableClient ) {
#ifdef __APPLE__
    syphonClient.bind();
    syphonClient.unbind();
    return syphonClient.getTexture();
#else
    return spoutReceiver.getTexture();
#endif
  }
  if (input != nullptr) {
    return input->getTexture();
  }
  if ( source ) {
    return source->getTexture();
  }
  return emptyTexture;
}

//--------------------------------------------------------------
//ofPixels& Input::processed() {
//  updateTextureIfNeeded();
//  return result;
//}

//--------------------------------------------------------------
void Input::updateGui() {
  if ( !isSetup ) {
    ofTexture& input = getTexture();
    isSetup = true;
  }
}
