//
//  Periscope.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 04/06/2016.
//
//

#pragma once

#include "Gui.h"
#include "OscSender.h"
#include "Component.h"

#define LOCAL_HOST "localhost" //"10.2.65.114"
#define LOCAL_PORT 9991

inline namespace PScope
{

class Periscope {
public:
  //!
	Periscope();
  //!
	~Periscope();
  //!
	void loadFromFile(string filePath);
  //!
	void saveToFile(string filePath);
  //!
	void loadGui();
  //!
	void addComponent(ComponentRef c);
  //!
	void setDebug(bool debug);
  //!
	bool& getDebug() { return debugMode; };
  //!
  void compute(cv::Mat &src);
  //!
	void draw();
	
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseMoved(int x, int y);
	
	static ofImage& getInput();
	ofTexture& getOutput();

private:
	bool debugMode;
	ofxPanel  gui;
	vector<unique_ptr<Thumbnail>> thumbnails;
	vector<ComponentRef> components;
  cv::Mat src;
	ofxOscSender sender;
	int mouseX, mouseY = 0;
};

}
