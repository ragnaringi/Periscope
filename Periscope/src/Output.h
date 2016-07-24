//
//  Output.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#ifndef Output_h
#define Output_h

#include <stdio.h>
#ifdef __APPLE__
#include "ofxSyphon.h"
#endif

inline namespace PScope
{

class Output {
public:
	Output();
	void send(ofTexture &src);
private:
#ifdef __APPLE__
	ofxSyphonServer syphonServer;
#endif
};
	
} /* namespace PScope */

#endif /* Output_h */
