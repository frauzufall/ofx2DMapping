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
}

void ofx2DMapping::showControls(bool show) {
    show_controls = show;
}

//void ofx2DMapping::setDirectEditMode(bool direct) {
//    view.setEditMode(direct);
//}

ofx2DMapping::~ofx2DMapping() {
}
