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
	if (debugMode) {
		int numComponents = components.size();
//		if (numComponents == 0) return;
		
		float width = source->getWidth();
		float height = source->getHeight();
//		source->draw(0, 0);//, width, height);
//		for (int i = 1; i < numComponents; i++) {
//			
//			auto c = components[i];
//			int x = (i %  (numComponents/2) == 0) ? width : 0;
//			int y = (i >= (numComponents/2)) ? height : 0;
//			
//			c->draw(x, y, width, height);
//		}
		if (numComponents == 0) {
			src.draw(0, 0);
			return;
		}
		components[0]->draw(0, 0);
		components[1]->draw(width, 0);
		components[2]->draw(width * 2, 0);
		
		components[3]->draw(0, height);
		components[4]->draw(width, height);
		components[5]->draw(width * 2, height);
		
		gui.draw();
		
		return;
	}
	
	source->draw(0, 0);
}
