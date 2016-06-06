
#include "ofApp.h"
#include "Periscope.h"

//--------------------------------------------------------------
void ofApp::setup(){
	
	ofSetWindowTitle("PERISCOPE");
	ofSetWindowShape(1152, 768);
	ofSetWindowPosition(40, 20);
	
	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	periscope.loadMovie("fingers.mov");
//	Threshold *t = new Threshold();
//	periscope.addComponent(t);
	
//	Classifier *c = new Classifier();
//	periscope.addComponent(c);
//	Background *b = new Background();
//	periscope.addComponent(b);
	Resize *resize = new Resize();
	GrayScale *grayScale = new GrayScale();
	Difference *d = new Difference();
	Threshold *t = new Threshold();
	Blur *blur = new Blur();
	Contours *c = new Contours();
	periscope.addComponent(resize);
	periscope.addComponent(grayScale);
	periscope.addComponent(d);
	periscope.addComponent(t);
	periscope.addComponent(blur);
	periscope.addComponent(c);
	
	// imitate() will set up previous and diff
	// so they have the same size and type as cam
	imitate(previous, cam);
	imitate(diff, cam);
	
//	lkMaxLevel.set("lkMaxLevel", 3, 0, 8);
//	lkMaxFeatures.set("lkMaxFeatures", 200, 1, 1000);
//	lkQualityLevel.set("lkQualityLevel", 0.01, 0.001, .02);
//	lkMinDistance.set("lkMinDistance", 4, 1, 16);
//	lkWinSize.set("lkWinSize", 8, 4, 64);
//	usefb.set("Use Farneback", false);
//	fbPyrScale.set("fbPyrScale", .5, 0, .99);
//	fbLevels.set("fbLevels", 4, 1, 8);
//	fbIterations.set("fbIterations", 2, 1, 8);
//	fbPolyN.set("fbPolyN", 7, 5, 10);
//	fbPolySigma.set("fbPolySigma", 1.5, 1.1, 2);
//	fbUseGaussian.set("fbUseGaussian", false);
//	fbWinSize.set("winSize", 32, 4, 64);
//	curFlow = &lk;
}

//--------------------------------------------------------------
void ofApp::update(){

	periscope.update();
	cam.update();
	return;
//	t->compute(nullptr);
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
		
		// Flow
//		if(usefb) {
//			curFlow = &fb;
//			fb.setPyramidScale(fbPyrScale);
//			fb.setNumLevels(fbLevels);
//			fb.setWindowSize(fbWinSize);
//			fb.setNumIterations(fbIterations);
//			fb.setPolyN(fbPolyN);
//			fb.setPolySigma(fbPolySigma);
//			fb.setUseGaussian(fbUseGaussian);
//		} else {
//			curFlow = &lk;
//			lk.setMaxFeatures(lkMaxFeatures);
//			lk.setQualityLevel(lkQualityLevel);
//			lk.setMinDistance(lkMinDistance);
//			lk.setWindowSize(lkWinSize);
//			lk.setMaxLevel(lkMaxLevel);
//		}
//			
//		// you can use Flow polymorphically
//		curFlow->calcOpticalFlow(cam);
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    
	ofSetColor(255);
	periscope.draw();
//	cam.draw(0, 0);
	return;
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
//	curFlow->draw(0,0);
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
