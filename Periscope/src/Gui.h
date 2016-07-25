//
//  Gui.h
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 25/07/2016.
//
//

#ifndef Gui_h
#define Gui_h

#include "ofxGui.h"
#include "ofGraphics.h"
#include "ofRectangle.h"

static const int THUMBNAIL_SIZE = 90;

inline namespace PScope {

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

}

#endif /* Gui_h */
