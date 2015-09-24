#pragma once

#include "ofMain.h"
#include "ofxPanel.h"
#include "ofx2DMappingProjector.h"
#include "ofxSortableList.h"

struct draggableVertex : public ofPoint {

    bool 	bBeingDragged;
    bool 	bOver;
    float 	radius;

};

struct mappableShape {

    mappableShape(){
        dst_bg.setFilled(true);
        dst_border.setFilled(false);
        dst_border.setStrokeWidth(2);
        src_border.setFilled(false);
        src_border.setStrokeWidth(2);
        dst_drag_unset.setFilled(false);
        dst_drag_unset.setStrokeWidth(2);
        dst_drag_set.setFilled(true);
        src_drag_unset.setFilled(false);
        src_drag_unset.setStrokeWidth(2);
        src_drag_unset.setStrokeColor(ofColor(255,255,255,200));
        src_drag_set.setFilled(true);
        src_drag_set.setFillColor(ofColor(255,255,255,200));
    }

    vector <draggableVertex> polyline, src;
    vector<ofPoint> dst;
    ofColor color;
    ofPath dst_border;
    ofPath dst_bg;
    ofPath dst_drag_unset;
    ofPath dst_drag_set;
    ofPath src_border;
    ofPath src_drag_unset;
    ofPath src_drag_set;

};


class ofx2DFormMapping : public ofxGuiGroup {

public:
    ofx2DFormMapping();
    ~ofx2DFormMapping();

    bool mouseMoved(ofMouseEventArgs &args);
    bool mouseDragged(ofMouseEventArgs &args);
    bool mousePressed(ofMouseEventArgs &args);
    bool mouseReleased(ofMouseEventArgs &args);
    bool mouseScrolled(ofMouseEventArgs &args);

    void setup(string title, ofPtr<ofx2DMappingProjector> parent_projector, ofxSortableList* parent_list, float w, float h);

    void rebuild();
    void updateForms();

    void generateDraw();
    void render();

    void setMappingBackground(ofFbo_ptr &fbo);
    void setOutputForm(ofRectangle rect);
    void setOutputForm(float x, float y, float w, float h);

    void setEditMode(bool direct_edit);

    virtual void setPosition(float x, float y);
    virtual void setSize(float w, float h);
    virtual void setShape(float x, float y, float w, float h);
    virtual void setShape(ofRectangle r);

    ofParameter<bool>& getShowSource();



private:

    void setMappingRects();
    void updateSourceBackground();

    void setZoomFactor(int factor);
    ofPoint addZoom(ofPoint p);
    float addZoom(float p);
    ofPoint removeZoom(ofPoint p);
    ofPoint addZoomRelativeOfDstRect(ofPoint p);
    ofPoint removeZoomRelativeOfDstRect(ofPoint p);

    ofParameter<bool> direct_edit;

    vector<mappableShape> shapes;
    ofRectangle control_rect, control_rect_backup;
    ofRectangle mapping_rect_dst, mapping_rect_src, mapping_rect_output;

    ofParameter<string> explanation;

    ofPtr<ofx2DMappingProjector> parent_projector;
    ofxSortableList* parent_list;

    int zoom_factor;
    float zoom_speed;
    ofPoint zoom_point, zoom_point_scaled, zoom_point_offset;
    ofPoint translation_dst;

    bool dragging_dst;
    ofPoint last_mouse;

    int mapping_margin;

    ofParameter<bool> show_source;

    //gui elements
    ofFbo_ptr mapping_bg;
    ofFbo mapping_front;
    ofTexture* source_bg;
    ofPath source_empty_bg;

};
