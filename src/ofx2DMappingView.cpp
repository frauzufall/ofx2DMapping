#include "ofx2DMappingView.h"

ofx2DMappingView::ofx2DMappingView():ofxPanel() {
    shape_dst.clear();
    shape_src.clear();

    shape_params.clear();
}

void ofx2DMappingView::setup(string title) {

    ofxPanel::setup(title);

}


bool ofx2DMappingView::mousePressed(ofMouseEventArgs &args) {

    return ofxPanel::mousePressed(args);

}

bool ofx2DMappingView::mouseDragged(ofMouseEventArgs &args) {

    return ofxPanel::mouseDragged(args);
}

bool ofx2DMappingView::mouseReleased(ofMouseEventArgs &args){

    return ofxPanel::mouseReleased(args);

}

bool ofx2DMappingView::mouseMoved(ofMouseEventArgs &args) {
    return ofxPanel::mouseMoved(args);
}

bool ofx2DMappingView::mouseScrolled(ofMouseEventArgs &args) {
    return ofxPanel::mouseScrolled(args);
}

ofxPanel& ofx2DMappingView::getList() {
    return shape_list;
}

ofxPanel& ofx2DMappingView::getControls() {
    return controls;
}
