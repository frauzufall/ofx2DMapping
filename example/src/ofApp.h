#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofx2DMapping.h"

#include "Scene1.h"
#include "Scene2.h"

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
    MappingContentShape_ptr cshape;

    Scene1 scene1;
    Scene2 scene2;

    bool show_controls;

};

