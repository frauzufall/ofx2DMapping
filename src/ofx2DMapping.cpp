#include "ofx2DMapping.h"

ofx2DMapping::ofx2DMapping() {
	show_controls = true;
}

void ofx2DMapping::setup() {
	setup("mapping/mapping.xml");

}

void ofx2DMapping::setup(string mapping_path) {
	ctrl.setup(mapping_path);
	view.setControl(&ctrl);
	view.setup(0,0,800,600);
}

void ofx2DMapping::update() {
	ctrl.update();
	if(show_controls) {
		view.update();
	}
}

ofx2DMappingController* ofx2DMapping::getControl() {
	return &ctrl;
}

ofx2DMappingView* ofx2DMapping::getControlView() {
	return &view;
}

void ofx2DMapping::draw() {
	ctrl.getOutput()->draw(ctrl.getOutputRectangle());

	if(show_controls) {
		//draw the control panel
		view.draw();
	}
}

void ofx2DMapping::setOutputShape(float x, float y, float width, float height) {
	ctrl.setOutputRectangle(ofRectangle(x,y,width,height));
	view.getFormMapping()->setOutputForm(x,y,width,height);
}

void ofx2DMapping::setControlShape(float x, float y, float width, float height) {
	view.setShape(ofRectangle(x,y,width,height));
	ctrl.setControlWidth(width);
	ctrl.setControlHeight(height);
}

void ofx2DMapping::showControls(bool show) {
	show_controls = show;
}

//void ofx2DMapping::setDirectEditMode(bool direct) {
//    view.setEditMode(direct);
//}

ofPtr<ofx2DMappingImage> ofx2DMapping::addImageTemplate(string name, string path) {
	ofPtr<ofx2DMappingImage> image = addTemplate<ofx2DMappingImage>(name);
	image->loadImage(path);
	image->setColor(ofColor(0,200,255));
	return image;
}

ofPtr<ofx2DMappingFbo> ofx2DMapping::addFboTemplate(string name, ofPtr<ofFbo> fbo) {
	ofPtr<ofx2DMappingFbo> content = addTemplate<ofx2DMappingFbo>(name);
	content->setFbo(fbo);
	return content;
}

ofPtr<ofx2DMappingColorShape> ofx2DMapping::addColorTemplate(string name, ofColor color) {
	ofPtr<ofx2DMappingColorShape> shape = addTemplate<ofx2DMappingColorShape>(name);
	shape->setColor(color);
	return shape;
}

ofx2DMapping::~ofx2DMapping() {
}
