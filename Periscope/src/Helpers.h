//
//  Helpers.hpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 20/07/2016.
//
//

#ifndef Helpers_h
#define Helpers_h

#include "Periscope.h"

PScope::Component* loadFromString(string title) {
	if      (title == "Resize")     return new Resize;
	else if (title == "Colours")    return new Colours;
	else if (title == "Greyscale")  return new Greyscale;
	else if (title == "Flow")       return new OpticalFlow;
	else if (title == "Difference") return new Difference;
	else if (title == "Threshold")  return new Threshold;
	else if (title == "Blur")       return new Blur;
	else if (title == "Contours")   return new Contours;
	else if (title == "Erode")      return new Erode;
	else if (title == "Dilate")     return new Dilate;
	else if (title == "Canny")      return new Canny;
	else if (title == "Sobel")      return new Sobel;
	else if (title == "Hough")      return new Hough;
	else assert(0);
}

#endif /* Helpers_hpp */
