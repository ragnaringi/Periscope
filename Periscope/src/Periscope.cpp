//
//  Periscope.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 04/06/2016.
//
//

#define HOST "localhost" //"10.2.65.114"
#define PORT 9999

#include "Periscope.h"

Periscope::Periscope() : debugMode(true)
{
	webCamBtn.addListener(this, &::Periscope::useWebcam);
	loadVidBtn.addListener(this, &::Periscope::openPanel);
	
	gui.setup();
	gui.add(loadVidBtn.setup("Load Video"));
	gui.add(webCamBtn.setup("Use Webcam"));
	gui.setPosition(900, 0);
	sender.setup(HOST, PORT);
	
	for (int i = 0; i < 6; i++) {
		unique_ptr<ThumbNail> t( new ThumbNail() );
		thumbNails.push_back(move(t));
	}
}

Periscope::~Periscope()
{
	components.clear();
}

void Periscope::openPanel()
{
	ofFileDialogResult result = ofSystemLoadDialog();
	cout << result.getPath() << endl;
	loadMovie(result.getPath());
}

void Periscope::loadMovie(string title)
{
	unique_ptr<ofVideoPlayer> player(new ofVideoPlayer);
	if (!player->load(title)) {
		cout << "Error loading movie: " << title << endl;
	}
	player->play();
	player->setLoopState(OF_LOOP_NORMAL);
	source = move(player);
}

void Periscope::useWebcam()
{
	unique_ptr<ofVideoGrabber> cam(new ofVideoGrabber);
	cam->setup(320, 240);
	source = move(cam);
}

void Periscope::addComponent(PeriscopeComponent *c_)
{
	c_->loadGui(&gui);
	c_->loadOsc(&sender);
	unique_ptr<PeriscopeComponent> c( c_ );
	components.push_back(move(c));
}

void Periscope::removeLast()
{
	components.pop_back();
}

void Periscope::setDebug(bool debug)
{
	debugMode = debug;
}

void Periscope::update()
{
	source->update();
	
	if(!source->isFrameNew()) return;
	
	copy(*source, src);
	src.update();
	
	for (auto const &c : components) {
		c->compute(src);
	}
}

void Periscope::draw()
{
	for (int i = thumbNails.size(); i --> 0;) {
		auto const &t = thumbNails[i];
		t->draw(i * 41, ofGetHeight() - 40);
	}
	
	if (!debugMode || components.size() == 0) {
		source->draw(0, 0);
		return;
	}
		
	float width = src.getWidth();
	float height = src.getHeight();

	int col = 0, row = 0;
	for (auto const &c : components) {
		c->draw(col * width, row * height);
		if ((col++ * width) > ofGetWidth() - width) {
			row++;
			col = 0;
		}
	}
	
	gui.draw();
}

//--------------------------------------------------------------
void Periscope::mouseMoved(int x, int y ){
	for (auto const &c : components) {
		c->setHighlighted(c->pointInside(x, y));
	}
	for (auto const &t : thumbNails) {
		t->setHighlighted(t->pointInside(x, y));
	}
}

//--------------------------------------------------------------
void Periscope::mousePressed(int x, int y, int button){
	for (int i = components.size(); i --> 0;) {
		auto const &c = components[i];
		if (c->pointInside(x, y)) {
			components.erase(components.begin() + i);
		}
	}
	
	for (int i = thumbNails.size(); i --> 0;) {
		auto const &t = thumbNails[i];
		if (t->pointInside(x, y)) {
			PeriscopeComponent *p;
			switch (i) {
				case 0:
					p = new Resize();
					break;
				case 1:
					p = new Colours();
					break;
				case 2:
					p = new Difference();
					break;
				case 3:
					p = new Threshold();
					break;
				case 4:
					p = new Blur();
					break;
				case 5:
					p = new Contours();
					break;
				default:
					break;
			}
			addComponent(p);
		}
	}
}
