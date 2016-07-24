//
//  Input.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#include "ofxCV.h"
#include "Input.h"

Input::Input() {
#ifdef __APPLE__
	// Syphon setup
	syphonClient.setup(); //using Syphon app Simple Server, found at http://syphon.v002.info/
	syphonClient.set("","Simple Server");
	syphonBuffer.allocate(320, 240, GL_RGBA);
#endif
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
		source->update();
		if( !source->isFrameNew() ) return;
		ofxCv::copy(*source, input);
	}
	else {
		syphonBuffer.begin();
		
		syphonClient.bind();
		ofClear(0.f);
		ofSetColor(ofColor::white);
		syphonClient.draw(0, 0, syphonBuffer.getWidth(), syphonBuffer.getHeight());
		syphonClient.unbind();
		
		syphonBuffer.end();
		
		if (syphonBuffer.isAllocated()) {
			ofPixels pix;
			syphonBuffer.readToPixels(pix);
			input.setFromPixels(pix);
		}
	}
}

ofImage& Input::getInput() {
	return input;
}
