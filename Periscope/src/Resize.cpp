//
//  Resize.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 19/10/2016.
//
//

#include "Resize.h"

inline namespace PScope {

void Resize::compute( cv::Mat &src ) {
  float newW = scale * src.cols;
  float newH = scale * src.rows;
  if ( newW > MAX_SIZE
    || newH > MAX_SIZE) {
    // Force resize if scaled output is larger than MAX_SIZE
    float greater = std::max( newW, newH );
    float wScale = MAX_SIZE / greater;
    cv::resize( src, src, cv::Size(), wScale, wScale );
  }
  else if ( scale < 1.f ) {
    cv::resize( src, src, cv::Size(), scale, scale );
  }
  ofxCv::copy( src, cpy );
};
  
//void Resize::compute( ofTexture &src ) {
//}

}
