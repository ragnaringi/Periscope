//
//  PixelSender.hpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 22/08/2016.
//
//

#ifndef PixelSender_h
#define PixelSender_h

#include "ofMain.h"

const int PixelStripWidth = 810;
class Impl;
class PixelSender {
public:
  PixelSender();
  ~PixelSender();
  void setup(string ipaddress, int port);
  void send(ofTexture& texture);
  void send(ofPixels& pixels);
private:
  int tcpPort;
  std::unique_ptr<Impl> pimpl;
  ofPixels pixels;
  ofFbo frameBuffer;
};

#endif /* PixelSender_h */
