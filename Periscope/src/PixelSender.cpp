//
//  PixelSender.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 23/08/2016.
//
//

#include "ofxNetwork.h"
#include "PixelSender.h"

struct Impl {
public:
  Impl() {
  }
  void setup(string ipaddress, int port) {
    client.setup(ipaddress, port);
    client.setMessageDelimiter("");
  }
  bool isConnected() {
    return client.isConnected();
  }
  void sendRawBytes(const char* rawBytes, const int numBytes) {
    client.sendRawBytes(rawBytes, numBytes);
  }
  ofxTCPClient client;
};

PixelSender::PixelSender() : pimpl( new Impl ) {
  frameBuffer.allocate(810, 1, GL_RGB);
}

PixelSender::~PixelSender() {
  
}

void PixelSender::setup(string ipaddress, int port) {
	pimpl->setup(ipaddress, port);
}

void PixelSender::send(ofTexture& texture) {
  // send pixel data over TCP
  if ( !pimpl->isConnected() ) return;
  // bind texture in framebuffer
  frameBuffer.begin();
  //    ofTranslate(0, -(texture.getHeight() - 1)); // draw the bottom pixel row only
  float scale = PixelStripWidth / texture.getWidth(); // scale image to line up with pixel strip
  texture.draw(0, 0, PixelStripWidth, texture.getHeight() * scale);
  frameBuffer.end();
  
  // read framebuffer texture into pixels
  frameBuffer.readToPixels(pixels);
  /*
   char buffer[pixels.getTotalBytes()];
   //    memcpy( buffer, pixels.getData(), pixels.getTotalBytes() );
   for (int i = 0; i < pixels.getTotalBytes(); i++) {
   
   buffer[i] = pixels.getData()[i];
   cout << buffer[i] << endl;
   }
   client.sendRawBytes( buffer, pixels.getTotalBytes() );
   */
}
void PixelSender::send(ofPixels& pixels) {
  int numPixels = 2430;
  char buffer[2430];
  //    memcpy( buffer, pixels.getData(), pixels.getTotalBytes() );
  int offset = 500000;
  for (int i = offset; i < numPixels + offset; i++) {
    buffer[i-offset] = pixels.getData()[i];
    //      cout << buffer[i] << endl;
  }
  pimpl->sendRawBytes( buffer, numPixels );
}
