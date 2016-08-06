//
//  ShapeDetector.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 06/08/2016.
//
//

#ifndef ShapeDetector_h
#define ShapeDetector_h

#include "ofxOsc.h"

inline namespace PScope
{
class ShapeDetector {
public:
  ShapeDetector();
  void update();
  void draw();
private:
  ofxOscReceiver receiver;
  std::vector<ofPolyline> contours;
};
  
} /* namespace PScope */

#endif /* ShapeDetector_h */
