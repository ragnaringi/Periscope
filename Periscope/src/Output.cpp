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
#endif
}

void Output::send(ofTexture &texture) {
  ofClear( 0 );
  ofSetColor( 255 );
#ifdef __APPLE__
	textureServer.publishTexture(&texture);
#else
	sender.sendTexture(texture, "Periscope Individual");
#endif
}

void Output::sendMain(ofTexture &texture) {
  ofClear( 0 );
  ofSetColor( 255 );
#ifdef __APPLE__
	mainServer.publishTexture(&texture);
#else
	sender.sendTexture(texture, "Periscope Main Out");
#endif
}