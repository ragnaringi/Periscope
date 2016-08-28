//
//  Input.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#include "Input.h"

static const int MAX_WIDTH = 1080;
static const int MAX_HEIGHT = 720;

void center(const ofRectangle &rect, int angle);
void applyRotation(const ofTexture &image, int angle);
void applyRotation(const ofRectangle &rect, int angle);
void center(const ofRectangle& rect, ofFbo& container, int angle);
void center(const ofTexture& texture, ofFbo& container, int angle);

//--------------------------------------------------------------
Input::Input() : isSetup(false), enabled(true), textureNeedsUpdate(false), angle(RotateNone) {
#ifdef __APPLE__
  // Syphon setup
  syphonClient.setup(); //using Syphon app Simple Server, found at http://syphon.v002.info/
  syphonClient.set("","Simple Server");
#else
  // Spout setup
  spoutReceiver.setup();
#endif
  frameBuffer.allocate(MAX_WIDTH, MAX_HEIGHT);
  result.allocate(MAX_WIDTH, MAX_HEIGHT, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void Input::loadMovie(std::string title) {
  input = nullptr;
  enableClient = false;
  unique_ptr<ofVideoPlayer> player(new ofVideoPlayer);
  if (!player->load(title)) {
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
    if (source != nullptr) {
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
  textureNeedsUpdate = true;
  updateGui();
}

//--------------------------------------------------------------
void Input::draw(bool fitToSize) {
  updateTextureIfNeeded();
  
  ofPushStyle();
  
  ofClear(0.f);
  ofSetColor(ofColor::white);
  
  ofTexture& input = raw();
  if ( !presentationMode ) {
  // Center images using original as anchor

    ofPushMatrix();
    if ( fitToSize ) {
      float scale = 1.f;
      if ( angle % 2 == 0 ) {
        // TODO:(Ragnar)
        scale = 1.f;
      }
      else {
        if ( input.getWidth() > ofGetHeight() ) {
          scale = ofGetHeight() / input.getWidth();
          ofTranslate((input.getWidth() - input.getHeight()) * 0.5, input.getWidth() / 8);
        }
      }
      ofScale(scale);
    }
    
    {
    ofPushMatrix();
    ofRectangle rect(0, 0, input.getWidth() * zoom, input.getHeight() * zoom);
    center(rect, angle);
    applyRotation(rect, angle);
    ofScale( zoom );
    input.draw(0, 0);
    ofPopMatrix(); /* Center images */
    }
    
    // Draw bounding box for crop
    {
    ofPushMatrix();
    ofRectangle rect = getCrop();
    center(rect, 0);
    ofNoFill();
    ofSetColor(ofColor::red);
    ofDrawRectangle(rect);
    ofPopMatrix();
    }
    
    ofPopMatrix();
  }
  else {
    float scale = (float)ofGetWidth() / (float)input.getWidth();
    input.draw(0, 0, ofGetWidth(), input.getHeight() * scale);
  }
  
  ofPopStyle();
}

//--------------------------------------------------------------
void Input::rotate(InputRotate angle_) {
  angle = angle_;
}

//--------------------------------------------------------------
ofRectangle Input::getCrop() {
  return ofRectangle(x, y, w, h);
}

//--------------------------------------------------------------
void Input::crop(int x_, int y_, int w_, int h_) {
  x = x_;
  y = y_;
  w = w_;
  h = h_;
}

void Input::centerCrop() {
  setCenter(0, 0);
}

void Input::setZoom(float zoom_) {
  zoom = zoom_;
}

void Input::fitCrop() {
  int w = fmin(raw().getWidth(), MAX_WIDTH);
  int h = fmin(raw().getHeight(), MAX_HEIGHT);
  if (angle % 2) std::swap(w, h);
  crop(0, 0, w, h);
}

void Input::setCenter(int x_, int y_) {
  crop(x_, y_, w, h);
}

//--------------------------------------------------------------
ofTexture& Input::raw() {
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
  if (source == nullptr) {
    return frameBuffer.getTexture();;
  }
  return source->getTexture();
}

//--------------------------------------------------------------
ofPixels& Input::processed() {
  updateTextureIfNeeded();
  return result;
}

//--------------------------------------------------------------
void Input::updateGui() {
  if (!isSetup) {
    ofTexture& input = raw();
    isSetup = true;
  }
}

//--------------------------------------------------------------
void Input::updateTextureIfNeeded() {
  if ( !textureNeedsUpdate ) return;
  
  if (frameBuffer.getWidth() != w ||
      frameBuffer.getHeight() != h) {
      frameBuffer.allocate(w, h);
  }
  
  // Rotate
  const ofTexture& texture = raw();
  frameBuffer.begin();
  ofClear(0);
  ofSetColor( ofColor::white );
  
  ofRectangle rect(0, 0, texture.getWidth() * zoom, texture.getHeight() * zoom);
  center(rect, frameBuffer, angle);
  applyRotation(rect, angle);
  ofScale( zoom );
  switch (angle) {
    case RotateNone:
      ofTranslate(-x, -y); break;
    case Rotate180:
      ofTranslate(x, y); break;
    case Rotate90:
      ofTranslate(-y, x); break;
    case Rotate270:
      ofTranslate(y, -x); break;
    default: break;
  }
  texture.draw(0, 0);
  frameBuffer.end();
  frameBuffer.readToPixels(result);	

  textureNeedsUpdate = false;
}

//--------------------------------------------------------------
void center(const ofRectangle& rect, int angle) {
  if (angle % 2 == 0) {
    ofTranslate(ofGetWidth()  * 0.5f - rect.getWidth()  * 0.5f,
                ofGetHeight() * 0.5f - rect.getHeight() * 0.5f);
  }
  else {
    ofTranslate(ofGetWidth()  * 0.5f - rect.getHeight() * 0.5f,
                ofGetHeight() * 0.5f - rect.getWidth()  * 0.5f);
  }
}

//--------------------------------------------------------------
void center(const ofRectangle& rect, ofFbo& container, int angle) {
  if (angle % 2 == 0) {
    ofTranslate(container.getWidth()  * 0.5f - rect.getWidth()  * 0.5f,
                container.getHeight() * 0.5f - rect.getHeight() * 0.5f);
  }
  else {
    ofTranslate(container.getWidth()  * 0.5f - rect.getHeight() * 0.5f,
                container.getHeight() * 0.5f - rect.getWidth()  * 0.5f);
  }
}

//--------------------------------------------------------------
void center(const ofTexture& texture, ofFbo& container,  int angle) {
  if (angle % 2 == 0) {
    ofTranslate(container.getWidth() * 0.5f - texture.getWidth()  * 0.5f,
                container.getHeight() * 0.5f - texture.getHeight() * 0.5f);
  }
  else {
    ofTranslate(container.getWidth() * 0.5f - texture.getHeight() * 0.5f,
                container.getHeight() * 0.5f - texture.getWidth()  * 0.5f);
  }
}

//--------------------------------------------------------------
void applyRotation(const ofTexture &texture, int angle) {
  ofRectangle rect(0, 0, texture.getWidth(), texture.getHeight());
  applyRotation(rect, angle);
}

//--------------------------------------------------------------
void applyRotation(const ofRectangle &rect, int angle) {
  ofRotate(angle * 90);
  
  switch (angle) {
      
    case Rotate90:
      ofTranslate( 0, -rect.getHeight() );
      break;
      
    case Rotate180:
      ofTranslate( -rect.getWidth(), -rect.getHeight() );
      break;
      
    case Rotate270:
      ofTranslate( -rect.getWidth(), 0 );
      break;
      
    default: break;
  }
}
