//
//  Periscope.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 04/06/2016.
//
//

#include "Periscope.h"

Periscope::Periscope() : debugMode(true)
{
	gui.setup();
	gui.setPosition(900, 0);
}

Periscope::~Periscope()
{
	for (auto c : components)
		delete c;
	components.clear();
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

void Periscope::addComponent(PeriscopeComponent *c)
{
	c->loadGui(&gui);
	components.push_back(c);
}

void Periscope::enableDebugMode(bool debug)
{
	debugMode = debug;
}

void Periscope::update()
{
	source->update();
	
	if(!source->isFrameNew()) return;
	
	copy(*source, src);
	src.update();
	
	for (auto c : components) {
		c->compute(src);
	}
}

void Periscope::draw()
{
	if (!debugMode || components.size() == 0) {
		source->draw(0, 0);
	}
		
	float width = src.getWidth();
	float height = src.getHeight();

	int col = 0, row = 0;
	for (int i = 0; i < components.size(); i++) {
		components[i]->draw(col * width, row * height);
		if ((col++ * width) > ofGetWidth()) {
			row++;
			col = 0;
		}
	}
	
	gui.draw();
}
