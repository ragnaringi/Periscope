//
//  Crop.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 19/10/2016.
//
//

#pragma once

#include "ofMain.h"
#include "constants.h"

inline namespace PScope {
  
namespace utils {

void center(const ofRectangle &rect, int angle);
void applyRotation(const ofTexture &image, int angle);
void applyRotation(const ofRectangle &rect, int angle);
void center(const ofRectangle& rect, ofFbo& container, int angle);
void center(const ofTexture& texture, ofFbo& container, int angle);
  
}

enum InputRotate {
	RotateNone,
	Rotate90,
	Rotate180,
	Rotate270
};

class Crop {
public:
  //!
  Crop() : zoom( 1.f ), angle( RotateNone ) {
    setCrop( MAX_WIDTH, MAX_HEIGHT );
    pixels.allocate( MAX_WIDTH, MAX_HEIGHT, OF_IMAGE_COLOR );
    frameBuffer.allocate( MAX_WIDTH, MAX_HEIGHT );
  }
  //!
  void compute( ofTexture &src ) {
    
    if ( shouldFit ) {
      int w = src.getWidth();
      int h = src.getHeight();
      if ( angle % 2 ) std::swap( w, h );
      setCrop( w, h );
      setCenter( 0, 0 );
      shouldFit = false;
    }
    
    if ( frameBuffer.getWidth() != w
        || frameBuffer.getHeight() != h) {
      frameBuffer.allocate(w, h);
    }
    
    // Rotate
    const ofTexture& texture = src;
    frameBuffer.begin();
    ofClear( 0 );
    ofSetColor( ofColor::white );
    
    ofRectangle rect( 0, 0, texture.getWidth() * zoom, texture.getHeight() * zoom );
    utils::center( rect, frameBuffer, angle );
    utils::applyRotation( rect, angle );
    ofScale( zoom, zoom );
    
    switch (angle) {
      case RotateNone:
        ofTranslate( -x, -y ); break;
      case Rotate180:
        ofTranslate( x, y ); break;
      case Rotate90:
        ofTranslate( -y, x ); break;
      case Rotate270:
        ofTranslate( y, -x ); break;
      default: break;
    }
    
    texture.draw( 0, 0 );
    frameBuffer.end();
    frameBuffer.readToPixels( pixels );
  }
  
  void drawCrop() {
    // Draw bounding box for crop
    ofPushMatrix();
    ofRectangle rect = getCrop();
    utils::center( rect, RotateNone );
    ofNoFill();
    ofSetColor( ofColor::red );
    ofDrawRectangle( rect );
    ofPopMatrix();
  }
  
  //--------------------------------------------------------------
  ofRectangle getCrop() {
    return ofRectangle(x, y, w, h);
  }
  
  //--------------------------------------------------------------
  void setCrop( ofRectangle& rect ) {
//    setCenter( rect.getX(), rect.getY() );
//    setC
  }
  
  //!
  void setCrop( int w, int h ) {
    this->w = w;
    this->h = h;
  }
  //!
  void setCenter( int x, int y ) {
    this->x = x;
    this->y = y;
  }
  
  //!
  void center() {
    setCenter( 0, 0 );
  }
  
  //!
  void fit() {
    shouldFit = true;
  }
  
  //!
  void setAngle( InputRotate angle ) {
    this->angle = angle;
  }
  
  //!
  void setZoom(float zoom_) {
    zoom = zoom_;
  }
  
  //!
  ofPixels& getPixels() {
    return pixels;
  }
  //!
  ofTexture& getTexture() {
    return frameBuffer.getTexture();
  }
  
private:
  int x, y, w, h;
  ofPixels pixels;
  ofFbo    frameBuffer;
  bool     textureNeedsUpdate;
  bool     shouldFit;
  float    zoom;
  int      angle;
};

}
