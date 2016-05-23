#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){

#ifdef _USE_LIVE_VIDEO
    cam.setVerbose(true);
    cam.setup(320,240);
#else
    cam.load("fingers.mov");
    cam.play();
    cam.setLoopState(OF_LOOP_NORMAL);
#endif
    
    // imitate() will set up previous and diff
    // so they have the same size and type as cam
    imitate(previous, cam);
    imitate(diff, cam);
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
    
    ofTranslate(320, 0);
    diff.draw(0, 0);
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
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
