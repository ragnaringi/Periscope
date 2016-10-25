//
//  Crop.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 19/10/2016.
//
//

#include "Crop.h"

inline namespace PScope {

//--------------------------------------------------------------
void utils::center(const ofRectangle& rect, int angle) {
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
void utils::center(const ofRectangle& rect, ofFbo& container, int angle) {
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
void utils::center(const ofTexture& texture, ofFbo& container,  int angle) {
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
void utils::applyRotation(const ofTexture &texture, int angle) {
  ofRectangle rect(0, 0, texture.getWidth(), texture.getHeight());
  applyRotation(rect, angle);
}

//--------------------------------------------------------------
void utils::applyRotation(const ofRectangle &rect, int angle) {
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
  
}
