//
//  Output.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#include "Output.h"

Output::Output() {
#ifdef __APPLE__
	syphonServer.setName("Periscope Output");
#else
	sender.init("Periscope Output");
#endif
}

void Output::send(ofTexture &texture) {
#ifdef __APPLE__
	syphonServer.publishTexture(&texture);
#else
	sender.send(texture);
#endif
}