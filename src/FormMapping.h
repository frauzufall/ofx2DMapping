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
    bool mouseScrolled(ofMouseEventArgs &args);

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
    void updateSourceBackground();

    void setZoomFactor(int factor);
    ofPoint addZoom(ofPoint p);
    float addZoom(float p);
    ofPoint removeZoom(ofPoint p);
    ofPoint addZoomRelativeOfDstRect(ofPoint p);
    ofPoint removeZoomRelativeOfDstRect(ofPoint p);

    bool direct_edit;

    ofFbo_ptr mapping_bg;
    ofFbo mapping_front;
    ofTexture* source_bg;

    vector<mappableShape> shapes;
    ofRectangle control_rect, control_rect_backup;
    ofRectangle mapping_rect_dst, mapping_rect_src, mapping_rect_output;

    ofParameter<string> explanation;

    Projector* parent_projector;
    ofxSortableList* parent_list;

    int zoom_factor;
    float zoom_speed;
    ofPoint zoom_point, zoom_point_scaled, zoom_point_offset;
    ofPoint translation_dst;

    bool dragging_dst;
    ofPoint last_mouse;

    int mapping_margin;

};
