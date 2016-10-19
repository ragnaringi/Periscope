//
//  Component.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 25/07/2016.
//
//

#pragma once

#include "Gui.h"
#include "ofxCv.h"
#include "ofxJSON.h"
#include "OscSender.h"

inline namespace PScope {
  
class Component : public MouseAware, public OscSender {
public:
  //!
  Component() {
    localGui.setup();
    localGui.add( bypass.set("Bypass", false) );
    localGui.add( close.set("Close", false) );
    localGui.add( useRaw.set("Use Raw", false) );
  }
  //!
  virtual ~Component() {};
  //!
  virtual void loadGui(ofxPanel *gui) = 0;
  
  //!
  virtual std::string getTitle() = 0;
  //!
  virtual void compute(cv::Mat &src) = 0;
  //!
  virtual void compute( ofTexture &src ) {}
  //!
  virtual void draw(int x, int y) {
    bounds.set(x, y, cpy.getWidth(), cpy.getHeight());
    ofSetColor(ofColor::white);
    if (highlight) ofSetColor(ofColor::red);
    ofTexture& tex = getTexture();
    tex.draw(x, y);
    ofDrawBitmapString(getTitle(), x + 10, y + 10);
    localGui.setPosition(x, y);
    localGui.draw();
  }
  //!
  bool selected = false;
  //!
  bool shouldClose() { return close; };
  //!
  bool shouldUseRaw() { return useRaw; };
  //!
  bool isBypassed() { return bypass; };
  ofTexture& getTexture() {
    cpy.update();
    return cpy.getTexture();
  }
  //!
  virtual void loadSettings( Json::Value settings ) {
    bypass = settings["Settings"][bypass.getName()].asBool();
    close  = settings["Settings"][close.getName()].asBool();
    useRaw = settings["Settings"][useRaw.getName()].asBool();
  }
  //!
  virtual ofxJSON getSettings() {
    ofxJSON settings;
    settings["Title"] = getTitle();
    settings["Settings"][bypass.getName()] = bypass.get();
    settings["Settings"][close.getName()]  = close.get();
    settings["Settings"][useRaw.getName()] = useRaw.get();
    return settings;
  };
  
protected:
  ofImage cpy;
  ofxPanel localGui;
  std::string title;
  std::vector<ofParameter<float>> output;
  
private:
  ofParameter<bool> bypass;
  ofParameter<bool> close;
  ofParameter<bool> useRaw;
};
  
  
using ComponentRef = std::shared_ptr<Component>;
  
  
//============================================================
// Misc components
//============================================================
  
#pragma mark - Grayscale
class Colours : public Component
{
public:
  Colours() {
//    output.push_back( ofParameter<float>("red", float) );
  }
  //!
  void loadGui( ofxPanel *gui ) {};
  //!
  void compute( cv::Mat &src ) {
    cv::Scalar colours = cv::mean( src );
    ofxCv::copy( src, cpy );
    
//    for ( auto const &s : output ) {
//      s = colours[0];
//    }
    
    // Send Osc
    ofxOscMessage m;
    m.setAddress( "/" + getTitle() );
    m.addFloatArg( colours[0] );
    m.addFloatArg( colours[1] );
    m.addFloatArg( colours[2] );
    sender->sendMessage( m );
  };
  //!
  string getTitle() {
    return "Colours";
  }
protected:
};

#pragma mark - Grayscale
class Greyscale : public Component {
public:
  //!
  void loadGui(ofxPanel *gui) {};
  //!
  void compute( cv::Mat &src ) {
    ofxCv::copyGray( src, cpy );
    ofxCv::copy( cpy, src );
  };
  //!
  string getTitle() {
    return "Greyscale";
  }
protected:
};

#pragma mark - Threshold
class Threshold : public Component {
public:
  Threshold() {
    thresh.set("Threshold", 128, 0, 255);
    autoThresh.set("Auto", false);
  }
  //!
  void loadGui(ofxPanel *gui) {
    gui->add(thresh);
    gui->add(autoThresh);
  }
  //!
  void compute(cv::Mat &src) {
    if(autoThresh) {
      ofxCv::autothreshold(src);
    } else {
      ofxCv::threshold(src, thresh);
    }
    ofxCv::copy(src, cpy);
  }
  //!
  string getTitle() {
    return "Threshold";
  }
  //!
  void loadSettings(Json::Value settings) {
    thresh     = settings["Settings"][thresh.getName()].asInt();
    autoThresh = settings["Settings"][autoThresh.getName()].asBool();
  }
  //!
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
class Difference : public Component {
public:
  //!
  Difference() {
    learn.set("Learn", true);
  }
  //!
  void loadGui(ofxPanel *gui) {
    gui->add(learn);
  }
  //!
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
  //!
  string getTitle() {
    return "Difference";
  }
  //!
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
class Sharpen : public Component {
public:
  //!
  Sharpen() {
    sigma.set("Sigma", 1, 0, 5);
    threshold.set("Threshold", 1, 0, 5);
    amount.set("Amount", 1, 0, 5);
  }
  //!
  void loadGui(ofxPanel *gui) {
    gui->add(sigma);
    gui->add(threshold);
    gui->add(amount);
  };
  //!
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
