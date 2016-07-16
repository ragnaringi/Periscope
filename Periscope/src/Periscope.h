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

static const int THUMBNAIL_SIZE = 90;

using namespace ofxCv;
using namespace cv;

class PeriscopeComponent;
class ThumbNail;

class Periscope {
public:
	Periscope();
	~Periscope();
	void openPanel();
	void loadGui();
	void loadMovie(string title);
	void selectWebCam();
	void addComponent(PeriscopeComponent *c);
	void removeLast();
	void setDebug(bool debug);
	bool& getDebug() { return debugMode; };
	void update();
	void draw();
	
	void mousePressed(int x, int y, int button);
	void mouseMoved(int x, int y);

private:
	bool debugMode;
	ofxPanel gui;
	ofParameter<bool> useWebCam, loadVideo;
	vector<unique_ptr<ThumbNail>> thumbNails;
	vector<unique_ptr<PeriscopeComponent>> components;
	unique_ptr<ofBaseVideoDraws> source;
	ofImage src;
	ofxOscSender sender;
};

class ThumbNail {
public:
	ThumbNail(string title) : title(title) {
		
	}
	virtual void draw(int x, int y) {
		this->x = x; this->y = y;
		ofSetColor(ofColor::white);
		ofDrawRectangle(x, y, THUMBNAIL_SIZE, THUMBNAIL_SIZE);
		ofSetColor(ofColor::black);
		if (highlight) ofSetColor(ofColor::gray);
		ofDrawRectangle(x+1, y+1, THUMBNAIL_SIZE-2, THUMBNAIL_SIZE-2);
		ofSetColor(ofColor::white);
		ofDrawBitmapString(title, x + 4, y + 20);
	}
	virtual bool pointInside(int x_, int y_) {
		return ((x_ > x && x_ < x + THUMBNAIL_SIZE)
						&& (y_ > y && y_ < y + THUMBNAIL_SIZE));
	}
	virtual void setHighlighted(bool h) { highlight = h; }
private:
	int x, y;
	bool highlight = false;
	string title;
};

class PeriscopeComponent {
public:
	PeriscopeComponent() {
		localGui.setup();
		localGui.add(bypass.set("Bypass", false));
		localGui.add(close.set("Close", false));
	}
	virtual ~PeriscopeComponent() {};
	virtual void loadGui(ofxPanel *gui) = 0;
	virtual void loadOsc(ofxOscSender *sender) {
		this->sender = sender;
	};
	virtual void compute(ofImage &src) = 0;
	virtual void draw(int x, int y) {
		this->x = x; this->y = y;
		ofSetColor(ofColor::white);
		if (cpy.isAllocated()) {
			if (highlight) ofSetColor(ofColor::red);
			cpy.update();
			cpy.draw(x, y);
		}
		ofDrawBitmapString(getDescription(), x + 10, y + 10);
		localGui.setPosition(x, y);
		localGui.draw();
	}
	virtual String getDescription() = 0;
	virtual bool pointInside(int x_, int y_) {
		return ((x_ > x && x_ < x + cpy.getWidth())
						&& (y_ > y && y_ < y + cpy.getHeight()));
	}
	virtual void setHighlighted(bool h) { highlight = h; }
	bool shouldClose() { return close; };
	bool isBypassed() { return bypass; };
protected:
	int x, y, w, h;
	ofParameter<bool> bypass;
	ofParameter<bool> close;
	bool highlight = false;
	ofImage cpy;
	ofxOscSender *sender;
	ofxPanel localGui;
};

#pragma mark - Resize
class Resize : public PeriscopeComponent
{
public:
	void loadGui(ofxPanel *gui) {
		gui->add(scale.set("Scale", 1, 0, 1));
	};
	void compute(ofImage &src) {
		src.resize(320, 240);
		cpy = src;
	};
	String getDescription() {
		return "Resize";
	}
protected:
	ofParameter<float> scale;
	int width = 0;
};

#pragma mark - Grayscale
class Colours : public PeriscopeComponent
{
public:
	void loadGui(ofxPanel *gui) {};
	void compute(ofImage &src) {
		cpy = src;
		
		cv::Scalar colours = mean(toCv(cpy));
		
		// Send Osc
		ofxOscMessage m;
		m.setAddress("/colours");
		m.addFloatArg(colours[0]);
		m.addFloatArg(colours[1]);
		m.addFloatArg(colours[2]);
		sender->sendMessage(m);
	};
	String getDescription() {
		return "Colours";
	}
protected:
};

#pragma mark - Grayscale
class GrayScale : public PeriscopeComponent
{
public:
	void loadGui(ofxPanel *gui) {};
	void compute(ofImage &src) {
		copyGray(src, cpy);
		src = cpy;
	};
	String getDescription() {
		return "Grayscale";
	}
protected:
};

#pragma mark - Blur
class Blur : public PeriscopeComponent
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
			blur(src, blurAmt);
		}
		copy(src, cpy);
	};
	String getDescription() {
		return "Blur";
	}
protected:
	ofParameter<int> blurAmt;
};

#pragma mark - Threshold
class Threshold : public PeriscopeComponent
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
		copyGray(src, cpy);
		if(autoT) {
			autothreshold(cpy);
		} else {
			threshold(cpy, t);
		}
		copy(cpy, src);
	}
	String getDescription() {
		return "Threshold";
	}
protected:
	ofParameter<int> t;
	ofParameter<bool> autoT;
};

#pragma mark - Difference
class Difference : public PeriscopeComponent
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
			copy(src, bg);
			learn = false;
		}
		// take the absolute difference of prev and current and save it inside diff
		absdiff(src, bg, cpy);
		src = cpy;
		
		cv::Scalar diffMean = mean(toCv(cpy));
		
		// Send Osc
		ofxOscMessage m;
		m.setAddress("/difference");
		m.addFloatArg(diffMean[0]);
		m.addFloatArg(diffMean[1]);
		m.addFloatArg(diffMean[2]);
		sender->sendMessage(m);
	}
	String getDescription() {
		return "Difference";
	}
protected:
	ofImage bg;
	ofParameter<bool> learn;
};

#pragma mark - Contours
class Contours : public PeriscopeComponent
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
		copyGray(src, cpy);
		contourFinder.setMinAreaRadius(minArea);
		contourFinder.setMaxAreaRadius(maxArea);
		contourFinder.findContours(cpy);
		contourFinder.setFindHoles(holes);
		copy(cpy, src);
		
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
		PeriscopeComponent::draw(x, y);
		ofPushMatrix();
		ofTranslate(x, y);
		ofSetColor(ofColor::red);
		contourFinder.draw();
		RectTracker& tracker = contourFinder.getTracker();
		if(showLabels) {
			ofSetColor(255);
			for(int i = 0; i < contourFinder.size(); i++) {
				ofPoint center = toOf(contourFinder.getCenter(i));
				ofPushMatrix();
				ofTranslate(center.x, center.y);
				int label = contourFinder.getLabel(i);
				string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
				ofDrawBitmapString(msg, 0, 0);
				ofVec2f velocity = toOf(contourFinder.getVelocity(i));
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
	String getDescription() {
		return "Contours";
	}
protected:
	ofParameter<int> minArea, maxArea;
	ofParameter<bool> holes, showLabels;
	ContourFinder contourFinder;
};

#pragma mark - Flow
class OpticalFlow : public PeriscopeComponent
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
		PeriscopeComponent::draw(x, y);
		ofPushMatrix();
		ofTranslate(x, y);
		curFlow->draw(0,0,cpy.getWidth(),cpy.getHeight());
		ofPopMatrix();
	}
	String getDescription() {
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
class Erode : public PeriscopeComponent
{
public:
	Erode() {
		iterations.set("Erode", 1, 0, 5);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(iterations);
	};
	void compute(ofImage &src) {
		erode(src, cpy, iterations);
		src = cpy;
	};
	String getDescription() {
		return "Erode";
	}
protected:
	ofParameter<int> iterations;
};

#pragma mark - Dilate
class Dilate : public PeriscopeComponent
{
public:
	Dilate() {
		iterations.set("Dilate", 1, 0, 5);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(iterations);
	};
	void compute(ofImage &src) {
		dilate(src, cpy, iterations);
		src = cpy;
	};
	String getDescription() {
		return "Dilate";
	}
protected:
		ofParameter<int> iterations;
};


#pragma mark - Canny Lines
class CannyEdges : public PeriscopeComponent
{
public:
	CannyEdges() {
		thresh1.set("Threshold 1", 1, 0, 200);
		thresh2.set("Threshold 2", 30, 0, 200);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(thresh1);
		gui->add(thresh2);
	};
	void compute(ofImage &src) {
		copyGray(src, cpy); // grayscale 8-bit input and output
		ofxCv::Canny(cpy, cpy, thresh1, thresh2);
		src = cpy;
	};
	String getDescription() {
		return "Canny";
	}
protected:
	ofParameter<int> thresh1;
	ofParameter<int> thresh2;
};

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
