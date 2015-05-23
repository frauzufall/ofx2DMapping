#include "ofx2DMapping.h"

ofx2DMapping::ofx2DMapping() {
}

void ofx2DMapping::setup(string mapping_path) {
    ctrl.setup(mapping_path);
    view.setControl(&ctrl);
}

void ofx2DMapping::update() {
    ctrl.update();
}

ofx2DMappingController* ofx2DMapping::getControl() {
    return &ctrl;
}

ofx2DMappingView* ofx2DMapping::getView() {
    return &view;
}

void ofx2DMapping::draw(float x, float y, float w, float h) {
    view.getMappingList()->setOutputForm(x,y,w,h);
    ctrl.getOutput()->draw(x,y,w,h);
}

ofx2DMapping::~ofx2DMapping() {
}
