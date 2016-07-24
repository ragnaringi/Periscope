//
//  Input.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#include "ofxCV.h"
#include "Input.h"

Input::Input() : isSetup(false), angle(RotateNone), x(0), y(0), w(0), h(0) {
#ifdef __APPLE__
	// Syphon setup
	syphonClient.setup(); //using Syphon app Simple Server, found at http://syphon.v002.info/
	syphonClient.set("","Simple Server");
	syphonBuffer.allocate(320, 240, GL_RGBA);
#else
	// Spout setup
#endif
	input.allocate(320, 240, OF_IMAGE_COLOR);
}

void Input::loadMovie(std::string title) {
	enableClient = false;
	unique_ptr<ofVideoPlayer> player(new ofVideoPlayer);
	if (!player->load(title)) {
		cout << "Error loading movie: " << title << endl;
	}
	player->play();
	player->setLoopState(OF_LOOP_NORMAL);
	source = move(player);
}

void Input::selectWebCam() {
	enableClient = false;
	unique_ptr<ofVideoGrabber> cam(new ofVideoGrabber);
	cam->setup(320, 240);
	source = move(cam);
}

void Input::selectSyphon() {
	enableClient = true;
}

void Input::update() {
	if ( !enableClient ) {
		if (source == nullptr) {
			result = input;
			return;
		}
		source->update();
		if( !source->isFrameNew() ) return;
		ofxCv::copy(*source, input);
	}
	else {
		syphonBuffer.begin();
#ifdef __APPLE__
		syphonClient.bind();
		ofClear(0.f);
		ofSetColor(ofColor::white);
		syphonClient.draw(0, 0, syphonBuffer.getWidth(), syphonBuffer.getHeight());
		syphonClient.unbind();
#endif
		syphonBuffer.end();
		
		if (syphonBuffer.isAllocated()) {
			ofPixels pix;
			syphonBuffer.readToPixels(pix);
			input.setFromPixels(pix);
		}
	}
	
	if (!isSetup && w == 0, h == 0) {
		crop(0, 0, input.getWidth(), input.getHeight());
	}
	
	// Rotate
	result = input;
	result.rotate90(angle);
	result.crop(x,y,w,h);
}

void Input::rotate(InputRotate angle_) {
	angle = angle_;
}

void Input::crop(int x_, int y_, int w_, int h_) {
	x = x_;
	y = y_;
	w = w_;
	h = h_;
}

ofImage& Input::getInput() {
	return result;
}
