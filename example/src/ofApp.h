#pragma once

#include "ofMain.h"
#include "ofxGuiExtended.h"
#include "ofx2DMapping.h"

#include "SomeScene.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
    void mouseMoved(ofMouseEventArgs &args);
    void mouseDragged(ofMouseEventArgs &args);
    void mousePressed(ofMouseEventArgs &args);
    void mouseReleased(ofMouseEventArgs &args);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

private:

    ofx2DMapping mapping;
    ofPtr<ofx2DMappingContentShape> cshape;

    SomeScene scene;

    bool show_controls;

};

