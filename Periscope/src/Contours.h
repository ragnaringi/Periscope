//
//  Contours.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 25/07/2016.
//
//

#ifndef Contours_h
#define Contours_h

#include "ofxGui.h"
#include "Component.h"

inline namespace PScope {

#pragma mark - Contour Finder

class Contours : public Component
{
public:
	Contours() {
		simplify.set("Simplify", false);
		mode.set("Mode", 0, 0, 4);
	}
	void loadGui(ofxPanel *gui) {
		gui->add(simplify);
		gui->add(mode);
	}
	void compute(ofImage &src) {
		ofxCv::copyGray(src, cpy);
		int simplifyMode = simplify ? CV_CHAIN_APPROX_SIMPLE : CV_CHAIN_APPROX_NONE;
		cv::findContours(ofxCv::toCv(cpy), contours, mode, simplifyMode);
		polylines.clear();
		for (auto const& p : contours) {
			polylines.push_back(ofxCv::toOf(p));
		}
		src = cpy;
		
		// Send Osc
		ofxOscMessage m;
		m.setAddress("/contours/size");
		m.addIntArg(contours.size());
		for (int i = 0; i < polylines.size(); ++i) {
			m.clear();
			m.setAddress("/contours/all");
			m.addIntArg(i);
			auto const& line = polylines[i];
			for (auto const& p : line.getVertices()) {
				m.addFloatArg(p.x);
				m.addFloatArg(p.y);
			}
			sender->sendMessage(m);
		}
	}
	void draw(int x, int y) {
		Component::draw(x, y);
		ofPushMatrix();
		ofTranslate(x, y);
		ofSetColor(ofColor::red);
		for (auto const& line : polylines) {
			line.draw();
		}
		ofPopMatrix();
	}
	string getTitle() {
		return "Contours";
	}
	void loadSettings(ofxJSON& settings) {
		mode			= settings["Settings"][mode.getName()].asInt();
		simplify  = settings["Settings"][simplify.getName()].asBool();
	}
	ofxJSON getSettings() {
		ofxJSON settings = Component::getSettings();
		settings["Settings"][mode.getName()] = mode.get();
		settings["Settings"][simplify.getName()] = simplify.get();
		return settings;
	};
protected:
	ofParameter<int> mode;
	ofParameter<bool> simplify, holes;
	std::vector<std::vector<cv::Point>> contours;
	std::vector<ofPolyline> polylines;
};
	
}

#endif /* Contours_h */
