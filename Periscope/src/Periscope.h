//
//  Periscope.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 04/06/2016.
//
//

#ifndef Periscope_h
#define Periscope_h

#include "Gui.h"
#include "Component.h"
#include "Contours.h"
#include "EdgeDetect.h"

#define HOST "localhost" //"10.2.65.114"
#define PORT 9999

inline namespace PScope
{

class Periscope {
public:
	Periscope();
	~Periscope();
	void loadFromFile(string filePath);
	void saveToFile(string filePath);
	void loadGui();
	void addComponent(Component *c);
	void setDebug(bool debug);
	bool& getDebug() { return debugMode; };
  void compute(cv::Mat &src);
	void draw();
	
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseMoved(int x, int y);
	
	static ofImage& getInput();
	ofTexture& getOutput();
private:
	bool debugMode;
	ofxPanel gui;
	vector<unique_ptr<Thumbnail>> thumbnails;
	vector<unique_ptr<Component>> components;
  cv::Mat src;
	ofxOscSender sender;
	int mouseX, mouseY = 0;
};

#pragma mark - Resize
class Resize : public Component
{
public:
	void loadGui(ofxPanel *gui) {
		gui->add(scale.set("Scale", 1, 0, 1));
	};
	void compute(cv::Mat &src) {
    cv::Size size(320, 240); // TODO
    cv::resize(src, src, size);
		ofxCv::copy(src, cpy);
	};
	string getTitle() {
		return "Resize";
	}
protected:
	ofParameter<float> scale;
	int width = 0;
};

#pragma mark - Grayscale
class Colours : public Component
{
public:
	void loadGui(ofxPanel *gui) {};
	void compute(cv::Mat &src) {
    cv::Scalar colours = cv::mean(src);
    ofxCv::copy(src, cpy);
		
		// Send Osc
		ofxOscMessage m;
		m.setAddress("/colours");
		m.addFloatArg(colours[0]);
		m.addFloatArg(colours[1]);
		m.addFloatArg(colours[2]);
		sender->sendMessage(m);
	};
	string getTitle() {
		return "Colours";
	}
protected:
};

#pragma mark - Grayscale
class Greyscale : public Component
{
public:
	void loadGui(ofxPanel *gui) {};
	void compute(cv::Mat &src) {
		ofxCv::copyGray(src, cpy);
    ofxCv::copy(cpy, src);
	};
	string getTitle() {
		return "Greyscale";
	}
protected:
};

#pragma mark - Blur
class Blur : public Component
{
public:
	Blur() {
		blurAmt.set("Blur", 5, 0, 25);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(blurAmt);
	};
	void compute(cv::Mat &src) {
		if (blurAmt > 0) {;
			ofxCv::blur(src, blurAmt);
		}
		ofxCv::copy(src, cpy);
	};
	string getTitle() {
		return "Blur";
	}
	void loadSettings(Json::Value settings) {
		blurAmt = settings["Settings"][blurAmt.getName()].asInt();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][blurAmt.getName()] = blurAmt.get();
		return settings;
	};
protected:
	ofParameter<int> blurAmt;
};

#pragma mark - Threshold
class Threshold : public Component
{
public:
	Threshold() {
		thresh.set("Threshold", 128, 0, 255);
		autoThresh.set("Auto", false);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(thresh);
		gui->add(autoThresh);
	}
	void compute(cv::Mat &src) {
		ofxCv::copyGray(src, cpy);
		if(autoThresh) {
			ofxCv::autothreshold(cpy);
		} else {
			ofxCv::threshold(cpy, thresh);
		}
		ofxCv::copy(cpy, src);
	}
	string getTitle() {
		return "Threshold";
	}
	void loadSettings(Json::Value settings) {
		thresh     = settings["Settings"][thresh.getName()].asInt();
		autoThresh = settings["Settings"][autoThresh.getName()].asBool();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][thresh.getName()] = thresh.get();
		settings["Settings"][autoThresh.getName()] = autoThresh.get();
		return settings;
	};
protected:
	ofParameter<int> thresh;
	ofParameter<bool> autoThresh;
};

#pragma mark - Difference
class Difference : public Component
{
public:
	Difference() {
		learn.set("Learn", true);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(learn);
	}
	void compute(cv::Mat &src) {
		if (learn) {
			ofxCv::copy(src, bg);
			learn = false;
		}
		// take the absolute difference of prev and current
		ofxCv::absdiff(src, bg, src);
		cv::Scalar diffMean = cv::mean(src);
    ofxCv::copy(src, cpy);
		
		// Send Osc
		ofxOscMessage m;
		m.setAddress("/difference");
		m.addFloatArg(diffMean[0]);
		m.addFloatArg(diffMean[1]);
		m.addFloatArg(diffMean[2]);
		sender->sendMessage(m);
	}
	string getTitle() {
		return "Difference";
	}
	void loadSettings(Json::Value settings) {
		learn = settings["Settings"][learn.getName()].asBool();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][learn.getName()] = learn.get();
		return settings;
	};
protected:
  cv::Mat bg;
	ofParameter<bool> learn;
};

#pragma mark - Flow
class OpticalFlow : public Component
{
public:
	OpticalFlow() {
		lkMaxLevel.set("lkMaxLevel", 3, 0, 8);
		lkMaxLevel.set("lkMaxLevel", 3, 0, 8);
		lkMaxFeatures.set("lkMaxFeatures", 200, 1, 1000);
		lkQualityLevel.set("lkQualityLevel", 0.01, 0.001, .02);
		lkMinDistance.set("lkMinDistance", 4, 1, 16);
		lkWinSize.set("lkWinSize", 8, 4, 64);
		fbPyrScale.set("fbPyrScale", .5, 0, .99);
		fbLevels.set("fbLevels", 4, 1, 8);
		fbIterations.set("fbIterations", 2, 1, 8);
		fbPolyN.set("fbPolyN", 7, 5, 10);
		fbPolySigma.set("fbPolySigma", 1.5, 1.1, 2);
		fbUseGaussian.set("fbUseGaussian", false);
		fbWinSize.set("winSize", 32, 4, 64);
		usefb.set("Use Farneback", true);
		
		curFlow = &fb;
	};
	void loadGui(ofxPanel *gui) {
	};
	void compute(cv::Mat &src) {
		ofxCv::copy(cpy, src);
		
		if(usefb) {
			curFlow = &fb;
			fb.setPyramidScale(fbPyrScale);
			fb.setNumLevels(fbLevels);
			fb.setWindowSize(fbWinSize);
			fb.setNumIterations(fbIterations);
			fb.setPolyN(fbPolyN);
			fb.setPolySigma(fbPolySigma);
			fb.setUseGaussian(fbUseGaussian);
		} else {
			curFlow = &lk;
			lk.setMaxFeatures(lkMaxFeatures);
			lk.setQualityLevel(lkQualityLevel);
			lk.setMinDistance(lkMinDistance);
			lk.setWindowSize(lkWinSize);
			lk.setMaxLevel(lkMaxLevel);
		}
		
		curFlow->calcOpticalFlow(cpy);
		
		// Send Osc
		ofVec2f flow = fb.getAverageFlow();
		ofxOscMessage m;
		m.setAddress("/flow");
		m.addFloatArg(flow.x);
		m.addFloatArg(flow.y);
		sender->sendMessage(m);
	};
	void draw(int x, int y) {
		Component::draw(x, y);
		ofPushMatrix();
		ofTranslate(x, y);
		curFlow->draw(0,0,cpy.getWidth(), cpy.getHeight());
		ofPopMatrix();
	}
	string getTitle() {
		return "Flow";
	}
protected:
	ofxCv::FlowFarneback fb;
	ofxCv::FlowPyrLK lk;
	
	ofxCv::Flow* curFlow;
	
	ofxPanel gui;
	ofParameter<float> fbPyrScale, lkQualityLevel, fbPolySigma;
	ofParameter<int> fbLevels, lkWinSize, fbIterations, fbPolyN, fbWinSize, lkMaxLevel, lkMaxFeatures, lkMinDistance;
	ofParameter<bool> fbUseGaussian, usefb;
};

#pragma mark - Erode
class Erode : public Component
{
public:
	Erode() {
		iterations.set("Erode", 1, 0, 5);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(iterations);
	};
	void compute(cv::Mat &src) {
		ofxCv::erode(src, iterations.get());
    ofxCv::copy(src, cpy);
	};
	string getTitle() {
		return "Erode";
	}
	void loadSettings(Json::Value settings) {
		iterations = settings["Settings"][iterations.getName()].asInt();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][iterations.getName()] = iterations.get();
		return settings;
	};
protected:
	ofParameter<int> iterations;
};

#pragma mark - Dilate
class Dilate : public Component
{
public:
	Dilate() {
		iterations.set("Dilate", 1, 0, 5);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(iterations);
	};
	void compute(cv::Mat &src) {
		ofxCv::dilate(src, iterations.get());
    ofxCv::copy(src, cpy);
	};
	string getTitle() {
		return "Dilate";
	}
	void loadSettings(Json::Value settings) {
		iterations = settings["Settings"][iterations.getName()].asInt();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][iterations.getName()] = iterations.get();
		return settings;
	};
protected:
		ofParameter<int> iterations;
};
	
#pragma mark - HoughLines
// Provide a binary image by running through Canny Edge detection or Threshold before HoughLines
// http://stackoverflow.com/questions/9310543/whats-the-use-of-canny-before-houghlines-opencv
// Default values are obtained from ofxCv::autorotate function in Helpers.h
class Hough : public Component
{
public:
	Hough() {
	}
	void loadGui(ofxPanel *gui) {
	};
	void compute(cv::Mat &src) {
		
		ofImage& raw = Periscope::getInput();
    cv::Mat srcMat = ofxCv::toCv(raw);
    cv::Mat& threshMat = src;
		
		lines.clear();
		double distanceResolution = 1;
		double angleResolution = CV_PI / 180;
		int voteThreshold = 10;
		double minLineLength = (srcMat.rows + srcMat.cols) / 8;
		double maxLineGap = 3;
		HoughLinesP(threshMat, lines, distanceResolution, angleResolution, voteThreshold, minLineLength, maxLineGap);
		
    ofxCv::copy(src, cpy);
	};
	void draw(int x, int y) {
		Component::draw(x, y);
		ofPushMatrix();
		ofTranslate(x, y);
		// draw lines
		ofSetColor(ofColor::red);
		for( size_t i = 0; i < lines.size(); i++ ) {
			ofDrawLine(lines[i][0], lines[i][1], lines[i][2], lines[i][3]);
		}
		ofPopMatrix();
	}
	string getTitle() {
		return "Hough Lines";
	}
protected:
	std::vector<cv::Vec4i> lines;
};

} /* namespace PScope */

#endif /* Periscope_h */

//#pragma mark - Filter
//class Filter : public PeriscopeComponent
//{
//public:
//	Filter() {};
//	void loadGui(ofxPanel *gui) {
//		gui->add(hipass.set("Hipass", 5, 0, 25));
//	};
//	void compute(cv::Mat &src) {
//	};
//	String getDescription() {
//		return "Filter";
//	}
//protected:
//	ofParameter<int> hipass;
//};
