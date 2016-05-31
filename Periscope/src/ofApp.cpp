
#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("PERISCOPE");
	ofSetWindowShape(1152, 768);
	ofSetWindowPosition(40, 20);
	
	ofSetVerticalSync(true);
	ofSetFrameRate(30);
	
	gui = new ofxDatGui( 0, 0 );
	gui->setAssetPath(ofToDataPath(""));
	gui->addHeader(":: Periscope Debug ::", false);
	rgbLabel = gui->addLabel("RGB: 0 | 0 | 0 | 0");
//	lightnessLabel = gui->addLabel("Lightness: 0");
//	brightnessLabel = gui->addLabel("Brightness: 0");
	gui->addFRM();

#ifdef _USE_LIVE_VIDEO
	cam.setVerbose(true);
	cam.setup(320,240);
#else
	cam.load("movie.mov");
	cam.play();
	cam.setLoopState(OF_LOOP_NORMAL);
#endif
	
	// imitate() will set up previous and diff
	// so they have the same size and type as cam
	imitate(previous, cam);
	imitate(diff, cam);
	
	
	// Countour following
	contourFinder.setMinAreaRadius(1);
	contourFinder.setMaxAreaRadius(200);
	contourFinder.setThreshold(30);
	// wait for half a frame before forgetting something
	contourFinder.getTracker().setPersistence(15);
	// an object can move up to 32 pixels per frame
	contourFinder.getTracker().setMaximumDistance(32);
	
	showLabels = true;
}

//--------------------------------------------------------------
void ofApp::update(){

	cam.update();
	if(cam.isFrameNew()) {
		// take the absolute difference of prev and cam and save it inside diff
		absdiff(cam, previous, diff);
		diff.update();
		
		// like ofSetPixels, but more concise and cross-toolkit
		copy(cam, previous);
		
		// mean() returns a Scalar. it's a cv:: function so we have to pass a Mat
//        diffMean = mean(toCv(diff));
		
		// you can only do math between Scalars,
		// but it's easy to make a Scalar from an int (shown here)
		// diffMean *= Scalar(50);
		
		// this is the key line: get the average of each column
		columnMean = meanCols(diff);
		
		// Contour following
		blur(cam, 10);
		contourFinder.findContours(cam); // TODO: Try with Diff
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    
	ofSetColor(255);
	cam.draw(0, 0);
//    diff.draw(320, 0);
//    
//    // use the [] operator to get elements from a Scalar
//    float diffRed = diffMean[0];
//    float diffGreen = diffMean[1];
//    float diffBlue = diffMean[2];
//    printf("Red %f \n", diffRed);
//    printf("Green %f \n", diffGreen);
//    printf("Blue %f \n", diffBlue);
//    
//    ofSetColor(255, 0, 0);
//    ofDrawRectangle(0, 0, diffRed, 10);
//    ofSetColor(0, 255, 0);
//    ofDrawRectangle(0, 15, diffGreen, 10);
//    ofSetColor(0, 0, 255);
//    ofDrawRectangle(0, 30, diffBlue, 10);
	
//	ofTranslate(0, 360);
//	diff.draw(0, 0);
	// draw the mean for each channel
	for(int k = 0; k < 3; k++) {
			// use the correct color for each channel
			switch(k) {
					case 0: ofSetColor(ofColor::red); break;
					case 1: ofSetColor(ofColor::blue); break;
					case 2: ofSetColor(ofColor::green); break;
			}
			
			ofNoFill();
			ofBeginShape();
			for(int i = 0; i < columnMean.rows; i++) {
					// Vec3b is one way of storing 24-bit (3 byte) colors
					Vec3b cur = columnMean.at<Vec3b>(i);
					ofVertex(i, cur[k]);
			}
			ofEndShape();
	}
	
	// Countour following
	ofSetBackgroundAuto(showLabels);
	RectTracker& tracker = contourFinder.getTracker();
	
	if(showLabels) {
		ofSetColor(255);
		contourFinder.draw();
		for(int i = 0; i < contourFinder.size(); i++) {
			ofPoint center = toOf(contourFinder.getCenter(i));
			ofPushMatrix();
			ofTranslate(center.x, center.y);
			int label = contourFinder.getLabel(i);
			string msg = ofToString(label) + ":" + ofToString(tracker.getAge(label));
			ofDrawBitmapString(msg, 0, 0);
			ofVec2f velocity = toOf(contourFinder.getVelocity(i));
			ofScale(5, 5);
			ofDrawLine(0, 0, velocity.x, velocity.y);
			ofPopMatrix();
		}
	} else {
		for(int i = 0; i < contourFinder.size(); i++) {
			unsigned int label = contourFinder.getLabel(i);
			// only draw a line if this is not a new label
			if(tracker.existsPrevious(label)) {
				// use the label to pick a random color
				ofSeedRandom(label << 24);
				ofSetColor(ofColor::fromHsb(ofRandom(255), 255, 255));
				// get the tracked object (cv::Rect) at current and previous position
				const cv::Rect& previous = tracker.getPrevious(label);
				const cv::Rect& current = tracker.getCurrent(label);
				// get the centers of the rectangles
				ofVec2f previousPosition(previous.x + previous.width / 2, previous.y + previous.height / 2);
				ofVec2f currentPosition(current.x + current.width / 2, current.y + current.height / 2);
				ofDrawLine(previousPosition, currentPosition);
			}
		}
	}
	
	// this chunk of code visualizes the creation and destruction of labels
	const vector<unsigned int>& currentLabels = tracker.getCurrentLabels();
	const vector<unsigned int>& previousLabels = tracker.getPreviousLabels();
	const vector<unsigned int>& newLabels = tracker.getNewLabels();
	const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
	ofSetColor(cyanPrint);
	for(int i = 0; i < currentLabels.size(); i++) {
		int j = currentLabels[i];
		ofDrawLine(j, 0, j, 4);
	}
	ofSetColor(magentaPrint);
	for(int i = 0; i < previousLabels.size(); i++) {
		int j = previousLabels[i];
		ofDrawLine(j, 4, j, 8);
	}
	ofSetColor(yellowPrint);
	for(int i = 0; i < newLabels.size(); i++) {
		int j = newLabels[i];
		ofDrawLine(j, 8, j, 12);
	}
	ofSetColor(ofColor::white);
	for(int i = 0; i < deadLabels.size(); i++) {
		int j = deadLabels[i];
		ofDrawLine(j, 12, j, 16);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key) {
		case ' ':
			cam.firstFrame();
			break;
		case 'f':
			ofToggleFullscreen();
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
