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
	mainServer.setName("Periscope Main");
	textureServer.setName("Periscope Processed");
#else
	sender.init("Periscope Output");
#endif
}

void Output::send(ofTexture &texture) {
#ifdef __APPLE__
	textureServer.publishTexture(&texture);
#else
	sender.send(texture);
#endif
}

void Output::sendMain(ofTexture &texture) {
#ifdef __APPLE__
	mainServer.publishTexture(&texture);
#else
	sender.send(texture);
#endif
}