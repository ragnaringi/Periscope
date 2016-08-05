//
//  Periscope.cpp
//  Periscope
//
//  Created by Ragnar Hrafnkelsson on 04/06/2016.
//
//

#define HOST "localhost" //"10.2.65.114"
#define PORT 9999

#include "Helpers.h"
#include "Periscope.h"

const int NUM_THUMBNAILS = 13;
string titles[NUM_THUMBNAILS] = {
	"Resize",
	"Colours",
	"Greyscale",
	"Flow",
	"Difference",
	"Threshold",
	"Blur",
	"Contours",
	"Erode",
	"Dilate",
	"Canny",
	"Sobel",
	"Hough"
};

static ofImage input; // TODO

//--------------------------------------------------------------
Periscope::Periscope() : debugMode(true) {
	sender.setup(HOST, PORT);
	
	for (int i = 0; i < NUM_THUMBNAILS; ++i) {
		unique_ptr<Thumbnail> t( new Thumbnail(titles[i]) );
		thumbnails.push_back(move(t));
	}
	gui.setup();
	loadGui();
}

//--------------------------------------------------------------
Periscope::~Periscope() {
	components.clear();
	thumbnails.clear();
}

//--------------------------------------------------------------
void Periscope::loadFromFile(string file) {
	
	ofxJSON result;
	
	bool parsingSuccessful = result.open(file);
	
	if (parsingSuccessful) {
		ofLogNotice("Periscope::loadFromFile") << result.getRawString();
		
		const Json::Value& periscope = result["Periscope"];
		if (periscope.isArray()) {
			for (Json::ArrayIndex i = 0; i < periscope.size(); ++i) {
				std::string title = periscope[i]["Title"].asString();
				Component* c = loadFromString(title);
				c->loadSettings(periscope[i]);
				addComponent(c);
			}
			loadGui();
		}
	}
	else ofLogError("Periscope::loadFromFile")  << "Failed to parse JSON" << endl;
}

//--------------------------------------------------------------
void Periscope::saveToFile(string file) {
	
	ofxJSON result;
	for (int i = 0; i < components.size(); ++i) {
		result["Periscope"][i] = components[i]->getSettings();
	}
	
	if (result.save(file, true)) {
		ofLogNotice("Periscope::saveToFile") << result.getRawString();
	}
	else ofLogError("Periscope::saveToFile") << "Failed to save JSON." << endl;
}

//--------------------------------------------------------------
void Periscope::loadGui() {
	gui.clear();
	gui.setPosition(ofGetWidth() - 200, 500);
	for (auto const &c : components) {
		c->loadGui(&gui);
	}
}

//--------------------------------------------------------------
void Periscope::addComponent(Component *c_) {
	c_->loadGui(&gui);
	c_->loadOsc(&sender);
	unique_ptr<Component> c( c_ );
	components.push_back(move(c));
}

//--------------------------------------------------------------
void Periscope::setDebug(bool debug) {
	debugMode = debug;
}

//--------------------------------------------------------------
void Periscope::compute(ofImage &src_) {
	input = src_;
	input.update();
	src = input;
	src.update();
	
	std::vector<int> toRemove; 
	
	for (int i = 0; i < components.size(); ++i) {
		auto const &c = components[i].get();
		if (!c->isBypassed()) {
			if (c->shouldUseRaw()) {
				ofxCv::copy(input, src);
			}
			c->compute(src);
		}
		if (c->shouldClose()) toRemove.push_back(i);
	}
	
	if (toRemove.size() > 0) {
		for (int i = toRemove.size(); i --> 0;) {
			components.erase(components.begin() + toRemove[i]);
		}
		loadGui();
	}
}

//--------------------------------------------------------------
void Periscope::draw() {
	ofSetBackgroundColor(ofColor::black);
	
	for (int i = thumbnails.size(); i --> 0;) {
		auto const &t = thumbnails[i];
		t->draw(i * (THUMBNAIL_SIZE + 1), ofGetHeight() - THUMBNAIL_SIZE);
	}
	
	if (!debugMode || components.size() == 0) {
		getInput().draw(0, 0);
		return;
	}
		
	float width  = src.getWidth();
	float height = src.getHeight();

	int col = 0, row = 0;
	for (auto const &c : components) {
		int x = col * width;
		int y = row * height;
		if (c->selected) {
			x = mouseX - (c->getBounds().getWidth() * 0.5f);
			y = mouseY - (c->getBounds().getHeight() * 0.5f);;
		}
		c->draw(x, y);
		if ((col++ * width) >= ofGetWidth() - width) {
			row++;
			col = 0;
		}
	}

	gui.draw();
}

//--------------------------------------------------------------
void Periscope::mouseDragged(int x, int y, int button) {
	mouseX = x; mouseY = y;
	
	for (int i = 0; i < components.size(); ++i) {
		
		auto const &c = components[i];
		if (!c->selected) continue;
		
		//
		// Horizontal
		//
		auto const &prev = components[ofClamp((i-1), 0, components.size()-1)];
		auto const &next = components[ofClamp((i+1), 0, components.size()-1)];
		
		// Moving left
		if (c->getBounds().getCenter().x < prev->getBounds().getRight()
				&& c->getBounds().getTop() >= prev->getBounds().getTop() - 20
				&& c->getBounds().getTop() <= prev->getBounds().getTop() + 20
				&& c != prev) {
			std::swap(components[i], components[i-1]);
			break;
		}
		// Moving right
		else if (c->getBounds().getCenter().x > next->getBounds().getLeft()
				&& c->getBounds().getTop() >= next->getBounds().getTop() - 20
				&& c->getBounds().getTop() <= next->getBounds().getTop() + 20
				&& c != next) {
			std::swap(components[i], components[i+1]);
			break;
		}
		//
		// Vertical
		//
		int centerY = c->getBounds().getCenter().y;
		// Moving up
		for (int j = 0; j < i; ++j) {
			auto const &anotherC = components[j];
			if (centerY <=  anotherC->getBounds().getBottom() - 30
					&& c->getBounds().getCenter().x >= anotherC->getBounds().getCenter().x - 20
					&& c->getBounds().getCenter().x <= anotherC->getBounds().getCenter().x + 20) {
				std::swap(components[i], components [j]);
				break;
			}
		}
		// Moving down
		for (int y = i+1; y < components.size(); ++y) {
			auto const &anotherC = components[y];
			if (centerY >=  anotherC->getBounds().getTop() + 30
					&& c->getBounds().getCenter().x >= anotherC->getBounds().getCenter().x - 20
					&& c->getBounds().getCenter().x <= anotherC->getBounds().getCenter().x + 20) {
				std::swap(components[i], components [y]);
				break;
			}
		}
	}
}

//--------------------------------------------------------------
void Periscope::mouseMoved(int x, int y) {
	mouseX = x; mouseY = y;
	for (auto const &c : components) {
		c->setHighlighted(c->pointInside(x, y));
	}
	for (auto const &t : thumbnails) {
		t->setHighlighted(t->pointInside(x, y));
	}
}

//--------------------------------------------------------------
void Periscope::mousePressed(int x, int y, int button) {
	mouseX = x; mouseY = y;
	for (int i = components.size(); i --> 0;) {
		auto const &c = components[i];
		if (!c->pointInside(x, y)) continue;
		c->selected = true;
		break;
	}
}

//--------------------------------------------------------------
void Periscope::mouseReleased(int x, int y, int button) {
	mouseX = x; mouseY = y;
	
	for (int i = components.size(); i --> 0;) {
		auto const &c = components[i];
		c->selected = false;
		if (c->pointInside(x, y)) {
			// TODO: Show full size
		}
	}
	
	for (int i = thumbnails.size(); i --> 0;) {
		auto const &t = thumbnails[i];
		if (t->pointInside(x, y)) {
			addComponent(loadFromString(t->getTitle()));
			loadGui();
			break;
		}
	}
}

//--------------------------------------------------------------
ofImage& Periscope::getInput() {
	return input;
}

//--------------------------------------------------------------
ofTexture& Periscope::getOutput() {
	if ( components.empty() ) {
		return getInput().getTexture();
	}
	return components.back()->getTexture();
}
