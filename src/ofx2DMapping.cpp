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

void ofx2DMapping::draw(float x, float y, float w, float h) {
    view.getMappingList()->setOutputForm(x,y,w,h);
    ctrl.getOutput()->draw(x,y,w,h);

    if(show_controls) {
        //draw the control panel
        view.draw();
    }
}

void ofx2DMapping::showControls(bool show) {
    show_controls = show;
}

ofx2DMapping::~ofx2DMapping() {
}
