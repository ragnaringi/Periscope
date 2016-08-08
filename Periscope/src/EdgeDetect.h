//
//  EdgeDetect.hp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 25/07/2016.
//
//

#ifndef EdgeDetect_h
#define EdgeDetect_h

#include "ofxGui.h"
#include "Component.h"

inline namespace PScope {

#pragma mark - Canny Edge Detection
	
class Canny : public Component {
public:
	Canny() {
		thresh1.set("Threshold 1", 1, 0, 200);
		thresh2.set("Threshold 2", 30, 0, 200);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(thresh1);
		gui->add(thresh2);
	};
  void compute(cv::Mat &src) {
		ofxCv::copyGray(src, cpy); // grayscale 8-bit input and output
		ofxCv::Canny(cpy, cpy, thresh1, thresh2);
		ofxCv::copy(cpy, src);
	};
	string getTitle() {
		return "Canny";
	}
	void loadSettings(Json::Value settings) {
		thresh1 = settings["Settings"][thresh1.getName()].asInt();
		thresh2 = settings["Settings"][thresh2.getName()].asInt();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][thresh1.getName()] = thresh2.get();
		settings["Settings"][thresh2.getName()] = thresh2.get();
		return settings;
	};
protected:
	ofParameter<int> thresh1;
	ofParameter<int> thresh2;
};

#pragma mark - Sobel Edge Detection
	
class Sobel : public Component {
public:
	Sobel() {
	}
	void loadGui(ofxPanel *gui) {
	};
  void compute(cv::Mat &src) {
		ofxCv::Sobel(src, cpy);
    ofxCv::copy(cpy, src);
	};
	string getTitle() {
		return "Sobel";
	}
protected:
};
	
}

#endif /* EdgeDetect_h */
