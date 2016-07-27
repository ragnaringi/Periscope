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
#else
#include "ofxSpout.h"
#endif

inline namespace PScope
{

class Output {
public:
	Output();
	void send(ofTexture &src);
	void sendMain(ofTexture &src);
private:
#ifdef __APPLE__
	ofxSyphonServer textureServer;
	ofxSyphonServer mainServer;
#else
	ofxSpout::Sender sender;
#endif
};
	
} /* namespace PScope */

#endif /* Output_h */
