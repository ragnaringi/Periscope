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

string titles[7] = {
"Resize",
"Colours",
"Flow",
"Difference",
"Threshold",
"Blur",
"Contours",
};

Periscope::Periscope() : debugMode(true)
{
	gui.setup();
	loadGui();
	sender.setup(HOST, PORT);
	
	for (int i = 0; i < 6; i++) {
		unique_ptr<ThumbNail> t( new ThumbNail(titles[i]) );
		thumbNails.push_back(move(t));
	}
}

Periscope::~Periscope()
{
	components.clear();
	thumbNails.clear();
}

void Periscope::loadGui()
{
	gui.clear();
	gui.setPosition(ofGetWidth(), 500);
	gui.add(useWebCam.set("Use WebCam", false));
	gui.add(loadVideo.set("Load Video", false));
	for (auto const &c : components) {
		c->loadGui(&gui);
	}
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

void Periscope::selectWebCam()
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
	if (useWebCam) {
		selectWebCam();
		useWebCam = false;
		loadVideo = false;
	}
	
	if (loadVideo) {
		openPanel();
		useWebCam = false;
		loadVideo = false;
	}
	
	source->update();
	
	if(!source->isFrameNew()) return;
	
	copy(*source, src);
	src.update();
	
	std::vector<int> toRemove; 
	
	for (int i = 0; i < components.size(); i++) {
		auto const &c = components[i];
		if (c->isBypassed()) continue;
		c->compute(src);
		if (c->shouldClose()) {
			toRemove.push_back(i);
		}
	}
	
	if (toRemove.size() > 0) {
		for (int i = toRemove.size(); i --> 0;) {
				components.erase(components.begin() + toRemove[i]);
		}
		loadGui();
	}
}

void Periscope::draw()
{
	for (int i = thumbNails.size(); i --> 0;) {
		auto const &t = thumbNails[i];
		t->draw(i * (THUMBNAIL_SIZE + 1), ofGetHeight() - THUMBNAIL_SIZE);
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
			// TODO: Show full size
			break;
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
					p = new OpticalFlow();
					break;
				case 3:
					p = new Difference();
					break;
				case 4:
					p = new Threshold();
					break;
				case 5:
					p = new Blur();
					break;
				case 6:
					p = new Contours();
					break;
				default:
					break;
			}
			addComponent(p);
			loadGui();
			break;
		}
	}
}
