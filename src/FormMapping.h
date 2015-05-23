#pragma once

#include "ofMain.h"
#include "ofxPanel.h"
#include "Projector.h"
#include "ofxSortableList.h"

struct draggableVertex : public ofPoint {

    bool 	bBeingDragged;
    bool 	bOver;
    float 	radius;

};

struct mappableShape {

    vector <draggableVertex> polyline, src;
    vector<ofPoint> dst;
    ofColor color;

};


class FormMapping : public ofxPanel {

public:
    FormMapping();

    bool mouseMoved(ofMouseEventArgs &args);
    bool mouseDragged(ofMouseEventArgs &args);
    bool mousePressed(ofMouseEventArgs &args);
    bool mouseReleased(ofMouseEventArgs &args);

    void setup(string title, Projector* parent_projector, ofxSortableList* parent_list, float w, float h);

    void rebuild();
    void updateForms();

    void update();
    void draw(bool show_source);

    void setMappingBackground(ofFbo_ptr &fbo);
    void setOutputForm(ofRectangle rect);
    void setOutputForm(float x, float y, float w, float h);

    void setEditMode(bool direct_edit);



private:

    void setMappingRects();

    bool direct_edit;

    ofFbo_ptr mapping_bg;
    ofTexture* source_bg;

    vector<mappableShape> shapes;
    ofRectangle control_rect, control_rect_backup;
    ofRectangle mapping_rect_dst, mapping_rect_src, mapping_rect_output;

    ofParameter<string> explanation;

    Projector* parent_projector;
    ofxSortableList* parent_list;

};
