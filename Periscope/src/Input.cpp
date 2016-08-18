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
  
  // Gui
  gui.setup();
  gui.add(x.set("x", 320, -MAX_WIDTH, MAX_WIDTH));
  gui.add(y.set("y", 240, -MAX_WIDTH, MAX_WIDTH));
  gui.add(w.set("w", MAX_WIDTH, 0, MAX_WIDTH));
  gui.add(h.set("h", MAX_HEIGHT, 0, MAX_HEIGHT));
  gui.add(angle.set("angle", 0, 0, Rotate270));
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
void Input::selectBlackmagic() {
  enableClient = false;
  source = nullptr;
  auto deviceList = ofxBlackmagic::Iterator::getDeviceList();
  if ( deviceList.empty() ) {
    cout << "No Blackmagic device found" << endl;
    return;
  }
  std::unique_ptr<ofxBlackmagic::Input> cam( new ofxBlackmagic::Input() );
  // NOTE: Mode is input device specific. Currently set to use with
  auto mode = bmdModeHD720p50; // GoPro Hero 720p stream
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
void Input::draw() {
  updateTextureIfNeeded();
  
  ofClear(0.f);
  ofSetColor(ofColor::white);
  
  ofTexture& input = raw();
  
  // Center images using original as anchor
  {
  ofPushMatrix();
  ofRectangle rect(0, 0, input.getWidth(), input.getHeight());
  center(rect, angle);
  applyRotation(input, angle);
  input.draw(0, 0);
  ofPopMatrix(); /* Center images */
  }
  
  // Draw bounding box for crop
  {
  ofPushMatrix();
  ofRectangle rect(x-1, y-1, w+2, h+2);
  center(rect, 0);
  ofNoFill();
  ofSetColor(ofColor::red);
  ofDrawRectangle(rect);
  ofPopMatrix();
  }
  
  gui.draw();
}

//--------------------------------------------------------------
void Input::rotate(InputRotate angle_) {
  angle = angle_;
}

//--------------------------------------------------------------
void Input::crop(int x_, int y_, int w_, int h_) {
  x = x_;
  y = y_;
  w = w_;
  h = h_;
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
    if (w != input.getWidth() || h != input.getHeight()) {
      crop(0, 0, fmin(input.getWidth(), MAX_WIDTH), fmin(input.getHeight(), MAX_HEIGHT));
    }
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
  center(texture, frameBuffer, angle);
  applyRotation(texture, angle);
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
  ofRotate(angle * 90);
  
  switch (angle) {
      
    case Rotate90:
      ofTranslate( 0, -texture.getHeight() );
      break;
      
    case Rotate180:
      ofTranslate( -texture.getWidth(), -texture.getHeight() );
      break;
      
    case Rotate270:
      ofTranslate( -texture.getWidth(), 0 );
      break;
      
    default: break;
  }
}
