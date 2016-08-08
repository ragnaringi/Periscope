//
//  Input.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#include "ofxCV.h"
#include "Input.h"

static const int MAX_WIDTH = 1080;
static const int MAX_HEIGHT = 720;

void center(ofTexture &image, int angle);
void applyRotation(ofTexture &image, int angle);

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
  gui.add(x.set("x", 320, 0, MAX_WIDTH));
  gui.add(y.set("y", 240, 0, MAX_WIDTH));
  gui.add(w.set("w", MAX_WIDTH, 0, MAX_WIDTH));
  gui.add(h.set("h", MAX_HEIGHT, 0, MAX_HEIGHT));
  gui.add(angle.set("angle", 0, 0, Rotate270));
}

//--------------------------------------------------------------
void Input::loadMovie(std::string title) {
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
  enableClient = false;
  unique_ptr<ofVideoGrabber> cam(new ofVideoGrabber);
  cam->setup(640, 480);
  source = move(cam);
  isSetup = false;
}

//--------------------------------------------------------------
void Input::selectSyphon(std::string server) {
#ifdef __APPLE__
  syphonClient.set("", server);
#endif
  source = nullptr;
  enableClient = true;
}

//--------------------------------------------------------------
void Input::update() {
  if ( enabled && !enableClient ) {
    if (source == nullptr) {
      return;
    }
    source->update();
    if( !source->isFrameNew() ) return;
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
  
  ofTexture& input = enableClient ? frameBuffer.getTexture() : source->getTexture();
  
  // Center images using original as anchor
  ofPushMatrix();
  center(input, angle);
  
  // Rotate original
  ofPushMatrix();
  applyRotation(input, angle);
  input.draw(0, 0);
  ofPopMatrix();
  
  // Draw bounding box for crop
  ofNoFill();
  ofSetColor(ofColor::red);
  ofDrawRectangle(x-1, y-1, w+2, h+2);
  
  ofPopMatrix(); /* Center images */
  
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
    return syphonClient.getTexture();
#else
    return spoutReceiver.getTexture();
#endif
  }
  return source->getTexture(); // TODO: Fix for Syphon/Spout
}

//--------------------------------------------------------------
ofPixels& Input::processed() {
  updateTextureIfNeeded();
  return result;
}

//--------------------------------------------------------------
void Input::updateGui() {
  if (!isSetup) {
    ofTexture& input = source->getTexture();
    if (w != input.getWidth() || h != input.getHeight()) {
      crop(0, 0, fmin(input.getWidth(), MAX_WIDTH), fmin(input.getHeight(), MAX_HEIGHT));
    }
    isSetup = true;
  }
}

//--------------------------------------------------------------
void Input::updateTextureIfNeeded() {
  if ( !textureNeedsUpdate ) return;
  
  if ( enableClient ) {
    
    frameBuffer.begin();
    ofClear(0);
    ofSetColor(ofColor::white);
    ofTexture *texture = nullptr;
#ifdef __APPLE__
    texture = &syphonClient.getTexture();
    syphonClient.draw(0, 0);
#else
    texture = &spoutReceiver.getTexture();
    texture->draw(0, 0);
#endif
    frameBuffer.end();
    
    // Resize framebuffer if necessary
    if (texture != nullptr &&
        frameBuffer.getWidth() != texture->getWidth() &&
        frameBuffer.getHeight() != texture->getHeight()) {
      frameBuffer.allocate(texture->getWidth(), texture->getHeight());
    }
    
    // Copy framebuffer to input
    if (frameBuffer.isAllocated()) {
      frameBuffer.readToPixels(result);
    }
  }
  
  if (frameBuffer.getWidth() != w ||
      frameBuffer.getHeight() != h) {
//    if (angle % 2 == 0) {
      frameBuffer.allocate(w, h);
//    }
//    else {
//      frameBuffer.allocate(h, w);
//    }
  }
  
  // Rotate
  ofTexture& texture = source->getTexture();
  frameBuffer.begin();
  ofClear(0);
  ofSetColor( ofColor::white );
  applyRotation(texture, angle);
  if (angle == 0) {
    texture.drawSubsection(0, 0, w, h, x, y);
  }
  else if (angle == 1) {
//    int x_ = texture.getWidth() - w;
//    int y_ = texture.getHeight() - h;
//    texture.drawSubsection(-y_, 0, h, w, y+y_, x);
  }
  else if (angle == 2) {
    int x_ = texture.getWidth() - w;
    int y_ = texture.getHeight() - h;
    texture.drawSubsection(x_, y_, w, h, -x+x_, -y+y_);
  }
  else {
//    int x_ = texture.getWidth() - w;
//    int y_ = texture.getHeight() - h;
//    texture.drawSubsection(y_, x_, h-y_, w-x_, y+y_, x+x_);
  }
  frameBuffer.end();
  frameBuffer.readToPixels(result);	

  textureNeedsUpdate = false;
}

//--------------------------------------------------------------
void center(ofTexture& texture, int angle) {
  if (angle % 2 == 0) {
    ofTranslate(ofGetWidth()  * 0.5f - texture.getWidth()  * 0.5f,
                ofGetHeight() * 0.5f - texture.getHeight() * 0.5f);
  }
  else {
    ofTranslate(ofGetWidth()  * 0.5f - texture.getHeight() * 0.5f,
                ofGetHeight() * 0.5f - texture.getWidth()  * 0.5f);
  }
}

//--------------------------------------------------------------
void applyRotation(ofTexture &texture, int angle) {
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
