#include "ofx2DMapping.h"

ofx2DMapping::ofx2DMapping() {
}

void ofx2DMapping::setup(string mapping_path, float w, float h) {
    ctrl.setup(mapping_path, w, h);
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

ofx2DMapping::~ofx2DMapping() {
}
