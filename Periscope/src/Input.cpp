//
//  Input.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 24/07/2016.
//
//

#include "ofxCV.h"
#include "Input.h"

Input::Input() : isSetup(false), angle(RotateNone) {
#ifdef __APPLE__
	// Syphon setup
	syphonClient.setup(); //using Syphon app Simple Server, found at http://syphon.v002.info/
	syphonClient.set("","Simple Server");
	syphonBuffer.allocate(320, 240, GL_RGBA);
#else
	// Spout setup
#endif
	input.allocate(320, 240, OF_IMAGE_COLOR);
	result = input;
	
	// Gui
	gui.setup();
	gui.add(x.set("x", 0, 0, 320));
	gui.add(y.set("y", 0, 0, 240));
	gui.add(w.set("w", 320, 0, 320));
	gui.add(h.set("h", 240, 0, 240));
	gui.add(angle.set("angle", 0, 0, 3));
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

void Input::selectSyphon(std::string server) {
	syphonClient.set("", server);
	enableClient = true;
}

void Input::update() {
	if ( !enableClient ) {
		if (source == nullptr) {
			return;
		}
		source->update();
		if( !source->isFrameNew() ) return;
		ofxCv::copy(*source, input);
		input.update();
	}
	else {
#ifdef __APPLE__
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
			input.update();
		}
#endif
	}
	
	if (!isSetup && w == 0, h == 0) {
		crop(0, 0, input.getWidth(), input.getHeight());
	}
	
	// Rotate
	result = input;
	result.rotate90(angle);
	result.crop(x,y,w,h);
}

void Input::draw() {
	// Center images
	ofPushMatrix();
	if (angle % 2 == 0) {
		ofTranslate(ofGetWidth()  * 0.5 - input.getWidth()  * 0.5,
								ofGetHeight() * 0.5 - input.getHeight() * 0.5);
	}
	else {
		ofTranslate(ofGetWidth() * 0.5 - input.getHeight() * 0.5,
								ofGetHeight()* 0.5 - input.getWidth()  * 0.5);
	}
	
	// Draw input with rotation
	ofPushMatrix();
	ofSetColor(ofColor::darkGray);
	ofRotate(angle * 90);
	switch (angle) {
  case RotateNone:
			input.draw(0,0);
			break;
	case Rotate90:
		input.draw(0,-input.getHeight());
		break;
	case Rotate180:
		input.draw(-input.getWidth(),-input.getHeight());
		break;
	case Rotate270:
		input.draw(-input.getWidth(),0);
		break;
  default:
			break;
	}
	ofPopMatrix();
	
	// Draw bounding box for crop
	ofSetColor(ofColor::red);
	ofDrawRectangle(x-1, y-1, w+2, h+2);
	
	// Draw processed copy
	ofSetColor(ofColor::white);
	result.draw(x, y);
	
	ofPopMatrix();
	
	gui.draw();
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

ofImage& Input::raw() {
	return input;
}

ofImage& Input::processed() {
	return result;
}
