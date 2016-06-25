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
}

//--------------------------------------------------------------
void Periscope::mousePressed(int x, int y, int button){
	for (int i = components.size(); i --> 0; ) {
		auto const &c = components[i];
		if (c->pointInside(x, y)) {
			components.erase(components.begin() + i);
		}
	}
}
