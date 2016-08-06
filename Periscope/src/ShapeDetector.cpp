//
//  ShapeDetector.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 06/08/2016.
//
//

#include "Periscope.h"
#include "ShapeDetector.h"

ShapeDetector::ShapeDetector() {
  receiver.setup(PORT);
}

void ShapeDetector::update() {
  
  while(receiver.hasWaitingMessages()){
  
    ofxOscMessage m;
    receiver.getNextMessage(m);
    
    if(m.getAddress() == "/contours/all"){
      contours.clear();
      int id = m.getArgAsInt(0);
      int numPoints = (m.getNumArgs() - 1) / 2;
      std::vector<cv::Point> points;
      for (int i = 1; i < numPoints; i++) {
        points.push_back(cv::Point(m.getArgAsFloat(i*2+1), m.getArgAsFloat(i*2)));
      }
      contours.push_back( ofxCv::toOf(points) );
    }
  }
}

void ShapeDetector::draw() {
  ofSetBackgroundColor(0);
  ofSetColor(255);
  
  for ( auto const& line : contours ) {
    line.draw();
  }
}