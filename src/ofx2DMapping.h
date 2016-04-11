#pragma once
#include "ofMain.h"
#include "ofx2DMappingController.h"
#include "ofx2DMappingView.h"

class ofx2DMapping {

public:
	ofx2DMapping();
	~ofx2DMapping();

	void setup();
	void setup(string mapping_path);
	void update();
	void draw();

	void showControls(bool show);

	ofx2DMappingController* getControl();
	ofx2DMappingView* addControlViewTo(ofx::DOM::Element* parent);
	ofx2DMappingView* addControlViewTo(ofx::DOM::Element& parent);
	ofx2DMappingView* addControlViewTo(ofxGui& parent);
	ofx2DMappingView* getControlView();

	void setOutputShape(ofRectangle shape);
	void setOutputShape(float x, float y, float width, float height);
	void setControlShape(ofRectangle shape);
	void setControlShape(float x, float y, float width, float height);

	//void setDirectEditMode(bool direct);

	template <class T>
	ofPtr<T> addTemplate(string name) {
		return ctrl.addTemplate<T>(name);
	}

	ofPtr<ofx2DMappingImage> addImageTemplate(string name, string path);
	ofPtr<ofx2DMappingFbo> addFboTemplate(string name, ofPtr<ofFbo> fbo);
	ofPtr<ofx2DMappingColorShape> addColorTemplate(string name, ofColor color);

private:

	ofx2DMappingController ctrl;
	ofx2DMappingView *view;

};
