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
