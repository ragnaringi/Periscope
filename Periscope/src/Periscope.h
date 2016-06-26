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
	virtual void draw(int x, int y) {
		this->x = x; this->y = y;
		ofSetColor(ofColor::blue);
		if (highlight) ofSetColor(ofColor::green);
		ofDrawRectangle(x, y, 40, 40);
	}
	virtual bool pointInside(int x_, int y_) {
		return ((x_ > x && x_ < x + 40)
						&& (y_ > y && y_ < y + 40));
	}
	virtual void setHighlighted(bool h) { highlight = h; }
private:
	int x, y;
	bool highlight = false;
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
		src.resize(320,240);
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
		
		ofPixels pix = cpy.getPixels();
		
		int width = pix.getWidth();
		int height = pix.getHeight();
		int numChannels = pix.getNumChannels();
		const int numPixels = width * height;
		
		unsigned char* pixels = pix.getData();
		
		float red = 0.f;
		float green = 0.f;
		float blue = 0.f;
		
		if (numChannels >= 3)
		{
			int totalR = 0;
			int totalG = 0;
			int totalB = 0;
			
			for (int i = 0; i < numPixels; i++)
			{
				int base = i * numChannels;
				
				unsigned char r = pixels[base];
				unsigned char g = pixels[base + 1];
				unsigned char b = pixels[base + 2];
				
				totalR	+= r;
				totalG	+= g;
				totalB	+= b;
			}
			
			red = totalR / numPixels;
			green = totalG / numPixels;
			blue = totalB / numPixels;
		}
		
		// Send Osc
		ofxOscMessage m;
		m.setAddress("/colours");
		m.addFloatArg(red);
		m.addFloatArg(green);
		m.addFloatArg(blue);
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
	void loadGui(ofxPanel *gui) {
		gui->add(blurAmt.set("Blur", 5, 0, 25));
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
	void loadGui(ofxPanel *gui) {
		gui->add(t.set("Threshold", 128, 0, 255));
		gui->add(autoT.set("Auto", false));
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
	void loadGui(ofxPanel *gui) {
		gui->add(learn.set("Learn", true));
	}
	void compute(ofImage &src) {
		if (learn) {
			copy(src, bg);
			learn = false;
		}
		// take the absolute difference of prev and current and save it inside diff
		absdiff(src, bg, cpy);
		src = cpy;
		
		cv::Scalar diffMean;
		diffMean = mean(toCv(cpy));
		
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
		contourFinder.setThreshold(0); // No thresholding as that's available as separete component
		// wait for half a frame before forgetting something
		contourFinder.getTracker().setPersistence(15);
		// an object can move up to 32 pixels per frame
		contourFinder.getTracker().setMaximumDistance(32);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(minArea.set("Min area", 10, 1, 100));
		gui->add(maxArea.set("Max area", 200, 1, 500));
		gui->add(holes.set("Holes", false));
		gui->add(showLabels.set("Show labels", false));
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
			// TODO: send bundle?
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

//#pragma mark - Erode
//class Erode : public PeriscopeComponent
//{
//public:
//	Erode() {};
//	void loadGui(ofxPanel *gui) {
//		gui->add(erode.set("Erode", 5, 0, 25));
//		gui->add(erode.set("Dilate", 5, 0, 25));
//	};
//	void compute(ofImage &src) {
//		copy(src, cpy);
//	};
//	String getDescription() {
//		return "Erode";
//	}
//protected:
//	ofParameter<int> erode;
//	ofParameter<int> dilate;
//};

#endif /* Periscope_h */
