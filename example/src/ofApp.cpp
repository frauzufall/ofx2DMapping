#include "ofApp.h"
#include "ofx2DMappingObjectFactory.h"
#include "Star.h"

//--------------------------------------------------------------
void ofApp::setup(){

	ofSetFrameRate(120);
	ofSetBackgroundColor(ofColor::black);

	show_controls = true;

	//setup some content that should get mapped
	scene.setup(1600, 900);

	//create template mapping objects that can be added afterwards via button

	//option to add shapes that show the content of a given fbo

	mapping.addFboTemplate("scene", scene.getFbo());

	//option to add black shapes
	mapping.addColorTemplate("black", ofColor(0));

	//option to add an image
	mapping.addImageTemplate("image","images/ente.jpg");

	//option to add custom class
	ofPtr<Star> star = mapping.addTemplate<Star>("star");
	star->pos = ofPoint(0.2, 0.2);

	//init mapping and load mapping settings from xml
	mapping.setup("mapping/mapping.xml");
	mapping.addControlViewTo(gui);

	//set output position and size
	mapping.setOutputShape(ofGetWidth()/2, 0, ofGetWidth()/2, ofGetHeight());

	//set position and size of control panel
	mapping.setControlShape(0, 0, ofGetWindowWidth()/2, ofGetWindowHeight());

}

//--------------------------------------------------------------
void ofApp::exit() {
}

//--------------------------------------------------------------
void ofApp::update() {

	//update the addon
	mapping.update();

	//update your content
	scene.update();

}

//--------------------------------------------------------------
void ofApp::draw(){

	//draw the mapped output image and the controls
	mapping.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch(key) {
	case 'c': {
		show_controls = !show_controls;
		mapping.showControls(show_controls);
		break;
	}
	case 'f': {
		ofToggleFullscreen();
		break;
	}
	default: break;
	}

}

//--------------------------------------------------------------
void ofApp::mouseMoved(ofMouseEventArgs &args){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(ofMouseEventArgs &args){
}

//--------------------------------------------------------------
void ofApp::mousePressed(ofMouseEventArgs &args){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(ofMouseEventArgs &args){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	//update output position and size
	mapping.setOutputShape(w/2, 0, w/2, h);

	//update position and size of control panel
	mapping.setControlShape(0, 0, w/2, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
