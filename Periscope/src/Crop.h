//
//  Crop.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 19/10/2016.
//
//

#pragma once

#include "constants.h"

/*
 void center(const ofRectangle &rect, int angle);
 void applyRotation(const ofTexture &image, int angle);
 void applyRotation(const ofRectangle &rect, int angle);
 void center(const ofRectangle& rect, ofFbo& container, int angle);
 void center(const ofTexture& texture, ofFbo& container, int angle);
 */

class Crop {
public:
  //!
  Crop() {
    result.allocate( MAX_WIDTH, MAX_HEIGHT, OF_IMAGE_COLOR );
    frameBuffer.allocate( MAX_WIDTH, MAX_HEIGHT );
  }
  //!
  void compute( ofTexture &src ) {
    
  }
  //!
  void setCrop( ofRect &rect ) {
    setCrop( rect.getX(), rect.getY(), rect.getW(), rect.getH() );
  }
  //!
  void setCrop( float x, y, w, h ){
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
  }
  //!
  ofPixels& processed() {
    return result;
  }
  //!
  ofTexture& raw() {
    return frameBuffer.getTexture();
  }
  /*
  //--------------------------------------------------------------
  void Input::updateTextureIfNeeded() {
    if ( !textureNeedsUpdate ) return;
    
    if ( frameBuffer.getWidth() != w
        || frameBuffer.getHeight() != h) {
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
    ofScale( zoom, zoom );
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
    frameBuffer.readToPixels( result );
    
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
  */
  
private:
  float x, y, w, h;
  ofPixels  result;
  ofFbo     frameBuffer;
};

/*

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
  resize->setScale( zoom_ );
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

*/

