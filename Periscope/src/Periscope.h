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

class PeriscopeComponent {
public:
	virtual ~PeriscopeComponent() {};
	virtual void loadGui(ofxPanel *gui) = 0; // TODO: Make protected
	virtual void loadOsc(ofxOscSender *sender) {
		this->sender = sender;
	};
	virtual void compute(ofImage &src) = 0;
	virtual void draw(int x, int y) {
		if (!cpy.isAllocated()) return;
		cpy.update();
		cpy.draw(x, y);
		ofSetColor(ofColor::white);
		ofDrawBitmapString(getDescription(), x + 10, y + 10);
	}
	virtual String getDescription() = 0;
protected:
	ofImage cpy;
	ofxOscSender *sender;
};

class Periscope {
public:
	Periscope();
	~Periscope();
	void loadMovie(string title);
	void useWebcam();
	void addComponent(PeriscopeComponent *c);
	void removeLast();
	void setDebug(bool debug);
	bool& getDebug() { return debugMode; };
	void update();
	void draw();
private:
	bool debugMode;
	ofxPanel gui;
	vector<unique_ptr<PeriscopeComponent>> components;
	unique_ptr<ofBaseVideoDraws> source;
	ofImage src;
	ofxOscSender sender;
};

#pragma mark - Resize
class Resize : public PeriscopeComponent
{
public:
	void loadGui(ofxPanel *gui) {
		gui->add(scale.set("Scale", 1, 0, 1));
	};
	void compute(ofImage &src)
	{
		src.resize(320,240);
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
		absdiff(src, bg, diff);
		src = diff;
		cpy = src;
		
		cv::Scalar diffMean;
		diffMean = mean(toCv(diff));
		cout << diffMean << endl;
		
		// Send Osc
		ofxOscMessage m;
		m.setAddress("/difference");
		m.addFloatArg(diffMean[0]);
		m.addFloatArg(diffMean[1]);
		m.addFloatArg(diffMean[2]);
		sender->sendMessage(m);
	}
	void draw(int x, int y) {
		diff.update();
		diff.draw(x, y);
		ofSetColor(ofColor::white);
		ofDrawBitmapString(getDescription(), x + 10, y + 10);
	}
	String getDescription() {
		return "Difference";
	}
protected:
	ofImage bg;
	ofImage diff;
	ofParameter<bool> learn;
};

#pragma mark - Contours
class Contours : public PeriscopeComponent
{
public:
	Contours() {
		contourFinder.setMinAreaRadius(1);
		contourFinder.setMaxAreaRadius(200);
		contourFinder.setThreshold(0);
		// wait for half a frame before forgetting something
		contourFinder.getTracker().setPersistence(15);
		// an object can move up to 32 pixels per frame
		contourFinder.getTracker().setMaximumDistance(32);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(minArea.set("Min area", 10, 1, 100));
		gui->add(maxArea.set("Max area", 200, 1, 500));
		gui->add(holes.set("Holes", false));
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
			m.setAddress("/contours/label");
			m.addIntArg(contourFinder.getLabel(i));
			b.addMessage(m);
			m.clear();
			m.setAddress("/contours/center");
			m.addFloatArg(contourFinder.getCenter(i).x / cpy.getWidth());
			m.addFloatArg(contourFinder.getCenter(i).y / cpy.getHeight());
			b.addMessage(m);
			m.clear();
			m.setAddress("/contours/velocity");
			m.addFloatArg(contourFinder.getVelocity(i)[0]);
			m.addFloatArg(contourFinder.getVelocity(i)[1]);
			b.addMessage(m);
			m.clear();
			m.setAddress("/contours/length");
			m.addIntArg(contourFinder.getArcLength(i));
			b.addMessage(m);
		}
		sender->sendBundle(b);
	}
	void draw(int x, int y) {
		PeriscopeComponent::draw(x, y);
		ofPushMatrix();
		ofTranslate(x, y);
		ofSetColor(ofColor::red);
		contourFinder.draw();
		ofPopMatrix();
	}
	String getDescription() {
		return "Contours";
	}
protected:
	ofParameter<int> minArea, maxArea;
	ofParameter<bool> holes;
	ContourFinder contourFinder;
};

/*
class Background : public PeriscopeComponent
{
public:
	Background() {
		
	}
	
	void loadGui(ofxPanel *gui) {
		gui->add(resetBackground.set("Reset Background", true));
		gui->add(learningTime.set("Learning Time", 0, 0, 30));
		gui->add(thresholdValue.set("Threshold Value", 10, 0, 255));
	}
	
	void compute(ofBaseVideoDraws &src) {
		if(resetBackground) {
			background.reset();
			resetBackground = false;
		}
		background.setLearningTime(learningTime);
		background.setThresholdValue(thresholdValue);
		background.update(src, thresholded);
		thresholded.update();
	}
	
	void draw(int x, int y, int w, int h) {
		if(thresholded.isAllocated()) {
			thresholded.draw(x, y);
		}
	}
	
protected:
	ofxCv::RunningBackground background;
	ofImage thresholded;
	ofParameter<bool> resetBackground;
	ofParameter<float> learningTime, thresholdValue;
};

class Classifier : public PeriscopeComponent
{
public:
	Classifier()
	{
		dst.allocate(640, 480, OF_IMAGE_GRAYSCALE);
		
		contourFinder.setMinAreaRadius(1);
		contourFinder.setMaxAreaRadius(200);
		contourFinder.setThreshold(30);
		// wait for half a frame before forgetting something
		contourFinder.getTracker().setPersistence(15);
		// an object can move up to 32 pixels per frame
		contourFinder.getTracker().setMaximumDistance(32);
	};
	
	void loadGui(ofxPanel *gui) {
		gui->add(minArea.set("Min area", 10, 1, 100));
		gui->add(maxArea.set("Max area", 200, 1, 500));
		gui->add(threshold.set("Threshold", 128, 0, 255));
		gui->add(blurAmt.set("Blur", 5, 0, 25));
		gui->add(holes.set("Holes", false));
	}
	
	void compute(ofBaseVideoDraws &src) {
//		contourFinder.findContours(src); // TODO: Try with Diff
		convertColor(src, dst, CV_RGB2GRAY);
		blur(dst, blurAmt);
		contourFinder.setMinAreaRadius(minArea);
		contourFinder.setMaxAreaRadius(maxArea);
		contourFinder.setThreshold(threshold);
		contourFinder.findContours(dst);
		contourFinder.setFindHoles(holes);
	}
	
	void draw(int x, int y, int w, int h) {
		contourFinder.draw();
		
		// finally, a report:
		ofSetHexColor(0xffffff);
		stringstream reportStr;
		reportStr << "bg subtraction and blob detection" << endl
		<< "press ' ' to capture bg" << endl
		<< "threshold " << threshold << " (press: +/-)" << endl
		<< "num blobs found " << contourFinder.size() << ", fps: " << ofGetFrameRate();
		ofDrawBitmapString(reportStr.str(), 20, 600);
//		ofSetBackgroundAuto(true);
//		RectTracker& tracker = contourFinder.getTracker();
//	
//		if(true) {
//			ofSetColor(255);
//			contourFinder.draw();
//			for(int i = 0; i < contourFinder.size(); i++) {
//				ofPoint center = toOf(contourFinder.getCenter(i));
//				ofPushMatrix();
//				ofTranslate(center.x, center.y);
//				int label = contourFinder.getLabel(i);
//				string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
//				ofDrawBitmapString(msg, 0, 0);
//				ofVec2f velocity = toOf(contourFinder.getVelocity(i));
//				ofScale(5, 5);
//				ofDrawLine(0, 0, velocity.x, velocity.y);
//				ofPopMatrix();
//			}
//		} else {
//			for(int i = 0; i < contourFinder.size(); i++) {
//				unsigned int label = contourFinder.getLabel(i);
//				// only draw a line if this is not a new label
//				if(tracker.existsPrevious(label)) {
//					// use the label to pick a random color
//					ofSeedRandom(label << 24);
//					ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
//					// get the tracked object (cv::Rect) at current and previous position
//					const cv::Rect& previous = tracker.getPrevious(label);
//					const cv::Rect& current = tracker.getCurrent(label);
//					// get the centers of the rectangles
//					ofVec2f previousPosition(previous.x + previous.width / 2, previous.y + previous.height / 2);
//					ofVec2f currentPosition(current.x + current.width / 2, current.y + current.height / 2);
//					ofDrawLine(previousPosition, currentPosition);
//				}
//			}
//		}
//	
//		// this chunk of code visualizes the creation and destruction of labels
//		const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
//		const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
//		const vector<unsigned int>& newLabels = tracker.getNewLabels();
//		const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
//		ofSetColor(cyanPrint);
//		for(int i = 0; i < currentLabels.size(); i++) {
//			int j = currentLabels[i];
//			ofDrawLine(j, 0, j, 4);
//		}
//		ofSetColor(magentaPrint);
//		for(int i = 0; i < previousLabels.size(); i++) {
//			int j = previousLabels[i];
//			ofDrawLine(j, 4, j, 8);
//		}
//		ofSetColor(yellowPrint);
//		for(int i = 0; i < newLabels.size(); i++) {
//			int j = newLabels[i];
//			ofDrawLine(j, 8, j, 12);
//		}
//		ofSetColor(ofColor::white);
//		for(int i = 0; i < deadLabels.size(); i++) {
//			int j = deadLabels[i];
//			ofDrawLine(j, 12, j, 16);
//		}
	}
protected:
	ofImage dst;
	ofParameter<int> minArea, maxArea, threshold, blurAmt;
	ofParameter<bool> holes;
	ofxCv::ContourFinder contourFinder;
};
 */

#endif /* Periscope_h */
