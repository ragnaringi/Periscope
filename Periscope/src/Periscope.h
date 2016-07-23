//
//  Periscope.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 04/06/2016.
//
//

#ifndef Periscope_h
#define Periscope_h

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxJSON.h"
#include "ofxSyphon.h"

static const int THUMBNAIL_SIZE = 90;

inline namespace PScope
{

class Component;
class Thumbnail;

class MouseAware {
public:
	ofRectangle& getBounds() {
		return bounds;
	}
	bool pointInside(int x, int y) {
		return getBounds().inside(x, y);
	}
	void setHighlighted(bool h) {
		highlight = h;
	}
protected:
	ofRectangle bounds;
	bool highlight = false;
};

class Periscope {
public:
	Periscope();
	~Periscope();
	void loadFromFile(string filePath);
	void saveToFile(string filePath);
	void loadGui();
	void loadMovie(string title);
	void selectWebCam();
	void addComponent(Component *c);
	void setDebug(bool debug);
	bool& getDebug() { return debugMode; };
	void update();
	void draw();
	
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseMoved(int x, int y);
	
	static ofImage& getInput();
private:
	void openPanel();
	bool debugMode;
	ofxPanel gui;
	ofParameter<bool> useWebCam, loadVideo, enableClient, enableServer;
	vector<unique_ptr<Thumbnail>> thumbnails;
	vector<unique_ptr<Component>> components;
	unique_ptr<ofBaseVideoDraws> source;
	ofImage src;
	ofxOscSender sender;
	int mouseX, mouseY = 0;
	
	// Syphon
	ofxSyphonClient syphonClient;
	ofxSyphonServer syphonServer;
	ofFbo syphonBuffer;
};

class Thumbnail : public MouseAware {
public:
	Thumbnail(string title) : title(title) {}
	virtual void draw(int x, int y) {
		bounds.set(x, y, THUMBNAIL_SIZE, THUMBNAIL_SIZE);
		ofSetColor(ofColor::white);
		ofDrawRectangle(bounds);
		ofSetColor(ofColor::black);
		if (highlight) ofSetColor(ofColor::gray);
		ofDrawRectangle(x+1, y+1, THUMBNAIL_SIZE-2, THUMBNAIL_SIZE-2);
		ofSetColor(ofColor::white);
		ofDrawBitmapString(title, x + 4, y + 20);
	}
	string& getTitle() { return title; };
private:
	string title;
};

class Component : public MouseAware {
public:
	Component() {
		localGui.setup();
		localGui.add(bypass.set("Bypass", false));
		localGui.add(close.set("Close", false));
		localGui.add(useRaw.set("Use Raw", false));
	}
	virtual ~Component() {};
	virtual void loadGui(ofxPanel *gui) = 0;
	virtual void loadOsc(ofxOscSender *sender) {
		this->sender = sender;
	};
	virtual string getTitle() = 0;
	virtual void compute(ofImage &src) = 0;
	virtual void draw(int x, int y) {
		bounds.set(x, y, cpy.getWidth(), cpy.getHeight());
		ofSetColor(ofColor::white);
		if (cpy.isAllocated()) {
			if (highlight) ofSetColor(ofColor::red);
			cpy.update();
			cpy.draw(x, y);
		}
		ofDrawBitmapString(getTitle(), x + 10, y + 10);
		localGui.setPosition(x, y);
		localGui.draw();
	}
	bool shouldClose() { return close; };
	bool shouldUseRaw() { return useRaw; };
	bool isBypassed() { return bypass; };
	ofTexture& getTexture() { return cpy.getTexture(); };
	bool selected = false;
	virtual void loadSettings(Json::Value settings) {
		bypass = settings["Settings"][bypass.getName()].asBool();
		close  = settings["Settings"][close.getName()].asBool();
		useRaw = settings["Settings"][useRaw.getName()].asBool();
	}
	virtual ofxJSON getSettings() {
		ofxJSON settings;
		settings["Title"] = getTitle();
		settings["Settings"][bypass.getName()] = bypass.get();
		settings["Settings"][close.getName()]  = close.get();
		settings["Settings"][useRaw.getName()] = useRaw.get();
		return settings;
	};
protected:
	ofParameter<bool> bypass;
	ofParameter<bool> close;
	ofParameter<bool> useRaw;
	ofImage cpy;
	ofxOscSender *sender;
	ofxPanel localGui;
};

#pragma mark - Resize
class Resize : public Component
{
public:
	void loadGui(ofxPanel *gui) {
		gui->add(scale.set("Scale", 1, 0, 1));
	};
	void compute(ofImage &src) {
		src.resize(320, 240);
		cpy = src;
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
	void compute(ofImage &src) {
		cpy = src;
		
		cv::Scalar colours = mean(ofxCv::toCv(cpy));
		
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
	void compute(ofImage &src) {
		ofxCv::copyGray(src, cpy);
		src = cpy;
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
	void compute(ofImage &src) {
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
		t.set("Threshold", 128, 0, 255);
		autoT.set("Auto", false);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(t);
		gui->add(autoT);
	}
	void compute(ofImage &src) {
		ofxCv::copyGray(src, cpy);
		if(autoT) {
			ofxCv::autothreshold(cpy);
		} else {
			ofxCv::threshold(cpy, t);
		}
		ofxCv::copy(cpy, src);
	}
	string getTitle() {
		return "Threshold";
	}
	void loadSettings(Json::Value settings) {
		t     = settings["Settings"][t.getName()].asInt();
		autoT = settings["Settings"][autoT.getName()].asBool();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][t.getName()] = t.get();
		settings["Settings"][autoT.getName()] = autoT.get();
		return settings;
	};
protected:
	ofParameter<int> t;
	ofParameter<bool> autoT;
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
	void compute(ofImage &src) {
		if (learn) {
			ofxCv::copy(src, bg);
			learn = false;
		}
		// take the absolute difference of prev and current and save it inside diff
		ofxCv::absdiff(src, bg, cpy);
		src = cpy;
		
		cv::Scalar diffMean = mean(ofxCv::toCv(cpy));
		
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
	ofImage bg;
	ofParameter<bool> learn;
};

#pragma mark - Contours
class Contours : public Component
{
public:
	Contours() {
		contourFinder.setMinAreaRadius(1);
		contourFinder.setMaxAreaRadius(200);
		contourFinder.setThreshold(0); // No thresholding as that's available as separate component
		// wait for half a frame before forgetting something
		contourFinder.getTracker().setPersistence(15);
		// an object can move up to 32 pixels per frame
		contourFinder.getTracker().setMaximumDistance(32);
		
		minArea.set("Min area", 10, 1, 100);
		maxArea.set("Max area", 200, 1, 500);
		holes.set("Holes", false);
		showLabels.set("Show labels", false);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(minArea);
		gui->add(maxArea);
		gui->add(holes);
		gui->add(showLabels);
	}
	void compute(ofImage &src) {
		ofxCv::copyGray(src, cpy);
		contourFinder.setMinAreaRadius(minArea);
		contourFinder.setMaxAreaRadius(maxArea);
		contourFinder.findContours(cpy);
		contourFinder.setFindHoles(holes);
		ofxCv::copy(cpy, src);
		
		// Send Osc
		ofxOscBundle b;
		ofxOscMessage m;
		m.setAddress("/contours/size");
		m.addIntArg(contourFinder.size());
		b.addMessage(m);
		for (int i = 0; i < contourFinder.size(); i++) {
			m.clear();
			m.setAddress("/contours/all");
			m.addIntArg(contourFinder.getLabel(i));
			// Normalised center
			m.addFloatArg(contourFinder.getCenter(i).x / cpy.getWidth());
			m.addFloatArg(contourFinder.getCenter(i).y / cpy.getHeight());
			// Velocity
			m.addFloatArg(contourFinder.getVelocity(i)[0]);
			m.addFloatArg(contourFinder.getVelocity(i)[1]);
			// Circumference
			m.addIntArg(contourFinder.getArcLength(i));
			sender->sendMessage(m);
		}
		sender->sendBundle(b);
	}
	void draw(int x, int y) {
		Component::draw(x, y);
		ofPushMatrix();
		ofTranslate(x, y);
		ofSetColor(ofColor::red);
		contourFinder.draw();
		ofxCv::RectTracker& tracker = contourFinder.getTracker();
		if(showLabels) {
			ofSetColor(255);
			for(int i = 0; i < contourFinder.size(); i++) {
				ofPoint center = ofxCv::toOf(contourFinder.getCenter(i));
				ofPushMatrix();
				ofTranslate(center.x, center.y);
				int label = contourFinder.getLabel(i);
				string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
				ofDrawBitmapString(msg, 0, 0);
				ofVec2f velocity = ofxCv::toOf(contourFinder.getVelocity(i));
				ofScale(5, 5);
				ofDrawLine(0, 0, velocity.x, velocity.y);
				ofPopMatrix();
			}
		} else {
			for(int i = 0; i < contourFinder.size(); i++) {
				unsigned int label = contourFinder.getLabel(i);
				// only draw a line if this is not a new label
				if(tracker.existsPrevious(label)) {
					// use the label to pick a random color
					ofSeedRandom(label << 24);
					ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
					// get the tracked object (cv::Rect) at current and previous position
					const cv::Rect& previous = tracker.getPrevious(label);
					const cv::Rect& current = tracker.getCurrent(label);
					// get the centers of the rectangles
					ofVec2f previousPosition(previous.x + previous.width / 2,
																	 previous.y + previous.height / 2);
					ofVec2f currentPosition(current.x + current.width / 2,
																	current.y + current.height / 2);
					ofDrawLine(previousPosition, currentPosition);
				}
			}
		}
		ofPopMatrix();
	}
	string getTitle() {
		return "Contours";
	}
	void loadSettings(ofxJSON& settings) {
		minArea    = settings["Settings"][minArea.getName()].asInt();
		maxArea    = settings["Settings"][maxArea.getName()].asInt();
		holes      = settings["Settings"][holes.getName()].asBool();
		showLabels = settings["Settings"][showLabels.getName()].asBool();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][minArea.getName()] = minArea.get();
		settings["Settings"][maxArea.getName()] = maxArea.get();
		settings["Settings"][holes.getName()] = holes.get();
		settings["Settings"][showLabels.getName()] = showLabels.get();
		return settings;
	};
protected:
	ofParameter<int> minArea, maxArea;
	ofParameter<bool> holes, showLabels;
	ofxCv::ContourFinder contourFinder;
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
	void compute(ofImage &src) {
		cpy = src;
		
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
		curFlow->draw(0,0,cpy.getWidth(),cpy.getHeight());
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
	void compute(ofImage &src) {
		ofxCv::erode(src, cpy, iterations);
		src = cpy;
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
	void compute(ofImage &src) {
		ofxCv::dilate(src, cpy, iterations);
		src = cpy;
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


#pragma mark - Canny Edge Detection
class Canny : public Component
{
public:
	Canny() {
		thresh1.set("Threshold 1", 1, 0, 200);
		thresh2.set("Threshold 2", 30, 0, 200);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(thresh1);
		gui->add(thresh2);
	};
	void compute(ofImage &src) {
		ofxCv::copyGray(src, cpy); // grayscale 8-bit input and output
		ofxCv::Canny(cpy, cpy, thresh1, thresh2);
		src = cpy;
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
class Sobel : public Component
{
public:
	Sobel() {
	}
	void loadGui(ofxPanel *gui) {
	};
	void compute(ofImage &src) {
		ofxCv::Sobel(src, cpy);
		src = cpy;
	};
	string getTitle() {
		return "Sobel";
	}
protected:
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
	void compute(ofImage &src) {
		cpy = src;
		ofImage& raw = Periscope::getInput();
		
		cv::Mat srcMat = ofxCv::toCv(raw), threshMat = ofxCv::toCv(cpy);
		
		lines.clear();
		double distanceResolution = 1;
		double angleResolution = CV_PI / 180;
		int voteThreshold = 10;
		double minLineLength = (srcMat.rows + srcMat.cols) / 8;
		double maxLineGap = 3;
		HoughLinesP(threshMat, lines, distanceResolution, angleResolution, voteThreshold, minLineLength, maxLineGap);
		
		src = cpy;
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

} /* namespace Periscope */

#endif /* Periscope_h */

//#pragma mark - Filter
//class Filter : public PeriscopeComponent
//{
//public:
//	Filter() {};
//	void loadGui(ofxPanel *gui) {
//		gui->add(hipass.set("Hipass", 5, 0, 25));
//	};
//	void compute(ofImage &src) {
//	};
//	String getDescription() {
//		return "Filter";
//	}
//protected:
//	ofParameter<int> hipass;
//};
