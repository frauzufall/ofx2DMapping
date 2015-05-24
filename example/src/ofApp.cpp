#include "ofApp.h"
#include "MappingObjectFactory.h"

//--------------------------------------------------------------
void ofApp::setup(){

    show_controls = true;

    //init mapping and load mapping settings from xml
    mapping.setup("mapping/mapping.xml");

    //setup some scene that creates an fbo that is to be mapped
    scene.setup(mapping.getControl()->contentWidth(), mapping.getControl()->contentHeight());

    //set input fbo
    mapping.getControl()->setInputFbo(scene.getFbo());

    //create template mapping objects that can then be added via button

    //option to add shapes that show the content of a given fbo
    mapping.getControl()->addTemplate<MappingContentShape>("content");
    //option to add black shapes
    ofPtr<MappingColorShape> black_shape = mapping.getControl()->addTemplate<MappingColorShape>("black");
    black_shape->color = ofColor(0);

    //set size of control panel
    mapping.getControlView()->setup(ofGetWindowWidth()/2, ofGetWindowHeight());

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

    ofSetColor(255);

    //draw the mapped output image and the controls
    mapping.draw(
                ofGetWidth()/2, 0,
                ofGetWidth()/2, ofGetHeight());

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
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	
}
