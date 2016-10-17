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
#include "OscSender.h"
#include "Component.h"
#include "Contours.h"
#include "EdgeDetect.h"
#include "Morphologic.h"

#define LOCAL_HOST "localhost" //"10.2.65.114"
#define LOCAL_PORT 9991

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
	ofxPanel  gui;
	vector<unique_ptr<Thumbnail>> thumbnails;
	vector<unique_ptr<Component>> components;
  cv::Mat src;
	ofxOscSender sender;
	int mouseX, mouseY = 0;
};
  
const float MAX_SIZE = 320.f;

#pragma mark - Resize
class Resize : public Component
{
public:
	void loadGui(ofxPanel *gui) {
		gui->add(scale.set("Scale", 1, 0, 1));
	};
	void compute(cv::Mat &src) {
    float newW = scale * src.cols;
    float newH = scale * src.rows;
    if (newW > MAX_SIZE ||
        newH > MAX_SIZE) {
      // Force resize if scaled output is larger than MAX_SIZE
      float greater = newW > newH ? newW : newH;
      float wScale = MAX_SIZE / greater;
      cv::resize(src, src, cv::Size(), wScale, wScale);
    }
    else if (scale < 1.f) {
      cv::resize(src, src, cv::Size(), scale, scale);
    }
		ofxCv::copy(src, cpy);
	};
	string getTitle() {
		return "Resize";
	}
protected:
	ofParameter<float> scale;
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
		if(autoThresh) {
			ofxCv::autothreshold(src);
		} else {
			ofxCv::threshold(src, thresh);
		}
		ofxCv::copy(src, cpy);
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
		usefb.set("Use Farneback", false);
		
		curFlow = &lk;
	};
	void loadGui(ofxPanel *gui) {
	};
	void compute(cv::Mat &src) {
		ofxCv::copy(src, cpy);
		
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
    float x = 0.f;
    std::vector<ofVec2f> flow = lk.getMotion();
    for (auto v : flow) {
      x += v.x;
//      cout << v << endl; 
    }
    x /= flow.size();
    cout << x << endl;
//		ofxOscMessage m;
//		m.setAddress("/flow");
//		m.addFloatArg(flow.x);
//		m.addFloatArg(flow.y);
//		sender->sendMessage(m);
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
  
// OpenCV cookbook page 56
static void sharpen(const cv::Mat &image, cv::Mat &result) {
  // allocate if necessary
  result.create(image.size(), image.type());
  for (int j= 1; j<image.rows-1; j++) { // for all rows
    // (except first and last)
    const uchar* previous=
    image.ptr<const uchar>(j-1); // previous row
    const uchar* current=
    image.ptr<const uchar>(j);     // current row
    const uchar* next=
    image.ptr<const uchar>(j+1); // next row
    uchar* output= result.ptr<uchar>(j); // output row
    for (int i=1; i<image.cols-1; i++) {
      *output++= cv::saturate_cast<uchar>(
                                          5*current[i]-current[i-1]
                                          -current[i+1]-previous[i]-next[i]);
    } }
  // Set the unprocess pixels to 0
  result.row(0).setTo(cv::Scalar(0));
  result.row(result.rows-1).setTo(cv::Scalar(0));
  result.col(0).setTo(cv::Scalar(0));
  result.col(result.cols-1).setTo(cv::Scalar(0));
}
  
static cv::Mat equalize(const cv::Mat &image) {
  cv::Mat result;
  cv::equalizeHist(image, result);
  return result;
}

  
#pragma mark - Grayscale
class Sharpen : public Component
{
public:
  Sharpen() {
    sigma.set("Sigma", 1, 0, 5);
    threshold.set("Threshold", 1, 0, 5);
    amount.set("Amount", 1, 0, 5);
  }
  void loadGui(ofxPanel *gui) {
    gui->add(sigma);
    gui->add(threshold);
    gui->add(amount);
  };
  void compute(cv::Mat &src) {
    // sharpen image using "unsharp mask" algorithm
//    cv::Mat blurred; double sigma = 1, threshold = 5, amount = 1;
//    cv::GaussianBlur(src, blurred, cv::Size(), sigma, sigma);
//    cv::Mat lowContrastMask = abs(src - blurred) < threshold;
//    cv::Mat sharpened = src * (1 + amount) + blurred * ( -amount );
//    ofxCv::copy(lowContrastMask, sharpened);
//    src.copyTo(src, sharpened);
//    ofxCv::copy(sharpened, src);
//    ofxCv::copy(src, cpy);
    
//    cv::Mat sharpened;
//    cv::GaussianBlur(src, sharpened, cv::Size(0, 0), 3);
//    cv::addWeighted(src, 1.5, sharpened, -0.5, 0, sharpened);
//    ofxCv::copy(sharpened, src);
//    ofxCv::copy(src, cpy);
    
//    cv::Mat sharpened;
//    sharpen(src, sharpened);
//    ofxCv::copy(sharpened, src);
//    ofxCv::copy(src, cpy);
//  
//    cv::Mat output;
//    ofxCv::copy(src, output);
//    linearTransform(output, src, 2.2, 50);
//    ofxCv::copy(src, cpy);
    
  };
  string getTitle() {
    return "Sharpen";
  }
protected:
  ofParameter<int> sigma;
  ofParameter<int> threshold;
  ofParameter<int> amount;
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
