#include "ofx2DFormMapping.h"

ofx2DFormMapping::ofx2DFormMapping(): ofxPanel() {

    shapes.clear();
    zoom_factor = 0;
    zoom_speed = 0.1;
    dragging_dst = false;
    mapping_margin = 10;

}

void ofx2DFormMapping::setup(string title, ofx2DMappingProjector *parent_projector, ofxSortableList *parent_list, float w, float h) {
    ofxPanel::setup(title);
    ofxPanel::setSize(w, h);
    control_rect = this->getShape();
    control_rect_backup = control_rect;
    this->parent_projector = parent_projector;
    this->parent_list = parent_list;
}

void ofx2DFormMapping::setMappingRects() {

    int margin = 10;
    int header = 20;

    if(direct_edit) {
        mapping_rect_dst = mapping_rect_output;
        mapping_rect_src.x = control_rect.x+margin;
        mapping_rect_src.y = control_rect.y+margin+header;
        mapping_rect_src.height = control_rect.height-margin-header;
        float output_ratio = parent_projector->outputWidth()/parent_projector->outputHeight();
        float output_ratio_inv = parent_projector->outputHeight()/parent_projector->outputWidth();
        mapping_rect_src.width = mapping_rect_src.height*output_ratio;
        if(mapping_rect_src.width > control_rect.width-2*margin) {
            mapping_rect_src.width = control_rect.width-2*margin;
            mapping_rect_src.height = mapping_rect_src.width*output_ratio_inv;
        }

        ofxPanel::setSize(mapping_rect_src.width+2*margin, header+mapping_rect_src.height+2*margin);
    }
    else {
        mapping_rect_dst.x = control_rect.x+margin;
        mapping_rect_dst.y = control_rect.y+margin+header;

        mapping_rect_dst.height = control_rect.height-margin-header;
        float output_ratio = parent_projector->outputWidth()/parent_projector->outputHeight();
        float output_ratio_inv = parent_projector->outputHeight()/parent_projector->outputWidth();
        mapping_rect_dst.width = mapping_rect_dst.height*output_ratio;
        if(mapping_rect_dst.width > control_rect.width-2*margin) {
            mapping_rect_dst.width = control_rect.width-2*margin;
            mapping_rect_dst.height = mapping_rect_dst.width*output_ratio_inv;
        }

        mapping_rect_src.x = mapping_rect_dst.x;
        mapping_rect_src.y = mapping_rect_dst.y+mapping_rect_dst.height+13;
        mapping_rect_src.width = mapping_rect_dst.width/2;
        float content_ratio = output_ratio_inv;
    //    float content_ratio = (float)Visuals::get().contentHeight()/(float)Visuals::get().contentWidth();
    //    float content_ratio_inv = (float)Visuals::get().contentWidth()/(float)Visuals::get().contentHeight();
        mapping_rect_src.height = mapping_rect_src.width*content_ratio;
    //    if(mapping_rect_src.height > control_rect.height*0.8-mapping_rect_dst.height) {
    //        mapping_rect_src.height = control_rect.height*0.8-mapping_rect_dst.height;
    //        mapping_rect_src.width = mapping_rect_src.height*content_ratio_inv;
    //    }

        ofxPanel::setSize(mapping_rect_dst.width+2*margin, mapping_rect_dst.height+header+mapping_rect_src.height+3*margin);

        mapping_front.clear();
        mapping_front.allocate(mapping_rect_dst.width+2*mapping_margin, mapping_rect_dst.height+2*mapping_margin, GL_RGBA);
    }
}

void ofx2DFormMapping::updateForms() {

    shapes.clear();

    for (uint i = 0; i < parent_projector->shapeCount(); i++) {

        ofPtr<ofx2DMappingObject> obj = parent_projector->getMappingObject(i);

        if(ofPtr<ofx2DMappingShape> shape = std::dynamic_pointer_cast<ofx2DMappingShape>(obj)) {

            mappableShape ms;
            ms.color = shape->color;
            ms.polyline.clear();
            ms.dst.clear();
            ms.src.clear();
            ofPolyline line = shape->polyline;

            for(uint j = 0; j < line.size(); j++) {
                ms.polyline.push_back(draggableVertex());
                ms.polyline[j].x = line[j].x*mapping_rect_dst.width+mapping_rect_dst.x;
                ms.polyline[j].y = line[j].y*mapping_rect_dst.height+mapping_rect_dst.y;
                ms.polyline[j].bOver 			= false;
                ms.polyline[j].bBeingDragged 	= false;
                ms.polyline[j].radius = 6;
            }

            for(int j = 0; j < 4; j++) {
                ms.dst.push_back(ofPoint());
                ms.dst[j].x = shape->dst[j].x*mapping_rect_dst.width+mapping_rect_dst.x;
                ms.dst[j].y = shape->dst[j].y*mapping_rect_dst.height+mapping_rect_dst.y;
            }

            if(ofPtr<ofx2DMappingContentShape> cshape = std::dynamic_pointer_cast<ofx2DMappingContentShape>(shape)) {
                for(int j = 0; j < 4; j++) {
                    ms.src.push_back(draggableVertex());
                    ms.src[j].x = cshape->src[j].x*mapping_rect_src.width+mapping_rect_src.x;
                    ms.src[j].y = cshape->src[j].y*mapping_rect_src.height+mapping_rect_src.y;
                    ms.src[j].bOver 			= false;
                    ms.src[j].bBeingDragged 	= false;
                    ms.src[j].radius = 6;
                }
            }

            shapes.push_back(ms);

        }
        else {
            if(ofPtr<ofx2DMappingPoint> point = std::dynamic_pointer_cast<ofx2DMappingPoint>(obj)) {

                mappableShape ms;
                ms.color = point->color;
                ms.src.clear();
                ms.dst.clear();
                ms.polyline.clear();
                draggableVertex v;
                v.x = point->pos.x*mapping_rect_dst.width+mapping_rect_dst.x;
                v.y = point->pos.y*mapping_rect_dst.height+mapping_rect_dst.y;
                v.bOver = false;
                v.bBeingDragged = false;
                v.radius = 13;
                ms.polyline.push_back(v);

                shapes.push_back(ms);

            }
        }

    }

    updateSourceBackground();

}

void ofx2DFormMapping::updateSourceBackground() {
    //find first content shape from top to use as background for source mapping

    source_bg = 0;
    vector<ofPtr<ofx2DMappingContentShape>> objs = parent_projector->getShapesByClass<ofx2DMappingContentShape>();
    if(objs.size() > 0) {
        for(int i = objs.size()-1; i >= 0; i--) {
            if(objs.at(i)->editable) {
                source_bg = objs.at(i)->getTexture();
                break;
            }
        }
    }
}

void ofx2DFormMapping::update() {

    if(control_rect.position != this->getPosition()) {
        control_rect.position = this->getPosition();
//        control_rect.setWidth(this->getWidth());
//        control_rect.setHeight(this->getHeight());

        rebuild();
    }

    updateSourceBackground();

}

void ofx2DFormMapping::rebuild() {
    setMappingRects();
    updateForms();
}

void ofx2DFormMapping::draw(bool show_source) {

    ofSetColor(42);
    ofFill();
    ofDrawRectangle(this->getPosition().x, this->getPosition().y, this->getWidth(), this->getHeight()-2);

    ofxPanel::draw();

    if(show_source) {
        //ofDrawRectangle(mapping_rect_src);
        if(source_bg) {
            ofSetColor(255,60);
            source_bg->draw(
                        mapping_rect_src.x,
                        mapping_rect_src.y,
                        mapping_rect_src.width,
                        mapping_rect_src.height);
        }
        else {
            ofSetColor(0,0,0,100);
            ofFill();
            ofDrawRectangle(mapping_rect_src);
        }
    }

    ofEnableAlphaBlending();

    ofSetLineWidth(2);

    //draw dst

    if(!direct_edit) {
        mapping_front.begin();
        ofClear(0,0,0,0);
        ofTranslate(mapping_margin, mapping_margin);

        ofPushMatrix();
        ofTranslate(-mapping_rect_dst.getPosition());

        //ZOOM TRANSLATION

        translation_dst = zoom_point-zoom_point_scaled+zoom_point_offset;
        if(translation_dst.x > 0) translation_dst.x = 0;
        if(translation_dst.y > 0) translation_dst.y = 0;
        if(translation_dst.x < -addZoom(mapping_rect_dst.getWidth())+mapping_rect_dst.getWidth())
            translation_dst.x = -addZoom(mapping_rect_dst.getWidth())+mapping_rect_dst.getWidth();
        if(translation_dst.y < -addZoom(mapping_rect_dst.getHeight())+mapping_rect_dst.getHeight())
            translation_dst.y = -addZoom(mapping_rect_dst.getHeight())+mapping_rect_dst.getHeight();
        ofTranslate(translation_dst);

        ofSetColor(255,160);
        mapping_bg->draw(
                    mapping_rect_dst.x,
                    mapping_rect_dst.y,
                    addZoom(mapping_rect_dst.width),
                    addZoom(mapping_rect_dst.height));

    }

    for (uint i = 0; i < shapes.size(); i++) {

//        //draw dst rectangles

//        ofNoFill();
//        ofSetColor(80,130,150);

//        ofBeginShape();
//        for(uint j = 0; j < shapes[i].dst.size(); j++) {
//            ofVertex(shapes[i].dst[j]);
//        }
//        ofEndShape(true);

        //draw shape lines and fillings

        for(int j = 0; j < 2; j++) {
            if(j == 0) {
                ofFill();
                ofSetColor(shapes[i].color,40);
            }
            else {
                ofNoFill();
                if(direct_edit) {
                    ofSetColor(255);
                }
                else {
                    ofSetColor(shapes[i].color,255);
                }

            }
            if(!(j == 0 && direct_edit)) {
                ofBeginShape();
                for(uint j = 0; j < shapes[i].polyline.size(); j++) {
                    ofVertex(addZoom(shapes[i].polyline[j].x-mapping_rect_dst.x)+mapping_rect_dst.x,
                             addZoom(shapes[i].polyline[j].y-mapping_rect_dst.y)+mapping_rect_dst.y);
                }
                ofEndShape(true);
            }
        }

        //draw dragging points
        ofSetColor(255,255,255,200);
        if(parent_projector->getMappingObject(i)->editable) {
            for(uint j = 0; j < shapes[i].polyline.size(); j++) {
                if (shapes[i].polyline[j].bOver) ofFill();
                else ofNoFill();
                ofDrawCircle(addZoom(shapes[i].polyline[j]-mapping_rect_dst.getPosition())+mapping_rect_dst.getPosition(),6);
            }
        }

    }

    if(!direct_edit) {
        ofPopMatrix();
        mapping_front.end();
        mapping_front.draw(mapping_rect_dst.getPosition()-ofPoint(mapping_margin, mapping_margin), mapping_front.getWidth(), mapping_front.getHeight());
    }

    //draw src

    for (uint i = 0; i < shapes.size(); i++) {

        ofNoFill();
        ofSetColor(shapes[i].color,255);
        ofBeginShape();

        for(uint j = 0; j < shapes[i].src.size(); j++) {
            ofVertex(shapes[i].src[j].x, shapes[i].src[j].y);
        }
        ofEndShape(true);

        //draw dragging points
        ofSetColor(255,255,255,200);
        if(parent_projector->getMappingObject(i)->editable) {
            for(uint j = 0; j < shapes[i].src.size(); j++) {

                if (shapes[i].src[j].bOver) ofFill();
                else ofNoFill();
                if(j%2==0)
                    ofDrawCircle(shapes[i].src[j].x, shapes[i].src[j].y,6);

            }
        }

    }

}

bool ofx2DFormMapping::mouseMoved(ofMouseEventArgs& args) {

    ofPoint mouse(args.x,args.y);
    for (uint i = 0; i < shapes.size(); i++){
        for (uint j = 0; j < shapes[i].polyline.size(); j++){
            ofPoint zoomed_p = addZoomRelativeOfDstRect(shapes[i].polyline[j]);
            float diffx = mouse.x - zoomed_p.x;
            float diffy = mouse.y - zoomed_p.y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < shapes[i].polyline[j].radius){
                shapes[i].polyline[j].bOver = true;
            } else {
                shapes[i].polyline[j].bOver = false;
            }
        }
        for (uint j = 0; j < shapes[i].src.size(); j++){
            float diffx = mouse.x - shapes[i].src[j].x;
            float diffy = mouse.y - shapes[i].src[j].y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < shapes[i].src[j].radius){
                shapes[i].src[j].bOver = true;
            } else {
                shapes[i].src[j].bOver = false;
            }
        }
    }

    return ofxPanel::mouseMoved(args);
}


bool ofx2DFormMapping::mouseDragged(ofMouseEventArgs &args) {

    ofPoint mouse(args.x,args.y);

    for (uint i = 0; i < shapes.size(); i++) {

        ofPtr<ofx2DMappingObject> obj = parent_projector->getMappingObject(i);

        for (uint j = 0; j < shapes[i].polyline.size(); j++) {

            if (shapes[i].polyline[j].bBeingDragged == true){

                ofPoint zoomed_mouse = removeZoomRelativeOfDstRect(mouse);
                ofPoint zoomed_mapping_rect_dst_lefttop = removeZoomRelativeOfDstRect(mapping_rect_dst.getPosition());
                ofPoint zoomed_mapping_rect_dst_rightbottom = removeZoomRelativeOfDstRect(mapping_rect_dst.getPosition()+ofPoint(mapping_rect_dst.getWidth(), mapping_rect_dst.getHeight()));

                if(mouse.x < mapping_rect_dst.x+mapping_rect_dst.width) {
                    if(mouse.x > mapping_rect_dst.x)
                        shapes[i].polyline[j].x = zoomed_mouse.x;
                    else
                        shapes[i].polyline[j].x = zoomed_mapping_rect_dst_lefttop.x;
                }
                else {
                    shapes[i].polyline[j].x = zoomed_mapping_rect_dst_rightbottom.x;
                }
                if(mouse.y < mapping_rect_dst.y+mapping_rect_dst.height) {
                    if(mouse.y > mapping_rect_dst.y)
                        shapes[i].polyline[j].y = zoomed_mouse.y;
                    else
                        shapes[i].polyline[j].y = zoomed_mapping_rect_dst_lefttop.y;
                }
                else {
                    shapes[i].polyline[j].y = zoomed_mapping_rect_dst_rightbottom.y;
                }

                if(shapes[i].polyline.size() == 4) {

                    if(ofPtr<ofx2DMappingShape> shape = std::dynamic_pointer_cast<ofx2DMappingShape>(obj)) {
                        shape->dst[j].x = (shapes[i].polyline[j].x-mapping_rect_dst.x)/mapping_rect_dst.width;
                        shape->dst[j].y = (shapes[i].polyline[j].y-mapping_rect_dst.y)/mapping_rect_dst.height;

                        shape->polyline[j].x = shape->dst[j].x;
                        shape->polyline[j].y = shape->dst[j].y;
                    }

                }
                else {

                    if(ofPtr<ofx2DMappingShape> shape = std::dynamic_pointer_cast<ofx2DMappingShape>(obj)) {
                        shape->polyline[j].x = (shapes[i].polyline[j].x-mapping_rect_dst.x)/mapping_rect_dst.width;
                        shape->polyline[j].y = (shapes[i].polyline[j].y-mapping_rect_dst.y)/mapping_rect_dst.height;

                        ofRectangle bounding = shape->polyline.getBoundingBox();

                        shape->dst[0].x = bounding.x;
                        shape->dst[0].y = bounding.y;
                        shape->dst[1].x = bounding.x+bounding.width;
                        shape->dst[1].y = bounding.y;
                        shape->dst[2].x = bounding.x+bounding.width;
                        shape->dst[2].y = bounding.y+bounding.height;
                        shape->dst[3].x = bounding.x;
                        shape->dst[3].y = bounding.y+bounding.height;
                    }
                    if(ofPtr<ofx2DMappingPoint> point = std::dynamic_pointer_cast<ofx2DMappingPoint>(obj)) {
                        //j should be 0
                        point->pos.x = (shapes[i].polyline[j].x-mapping_rect_dst.x)/mapping_rect_dst.width;
                        point->pos.y = (shapes[i].polyline[j].y-mapping_rect_dst.y)/mapping_rect_dst.height;

                    }

                }

                obj->newpos = true;
                parent_list->getListItems().at(shapes.size()-1-i)->setBackgroundColor(ofColor(0,200,210));
//                    Visuals::get().reloadLinesFromRaw();
            }
        }

        if(ofPtr<ofx2DMappingContentShape> cshape = std::dynamic_pointer_cast<ofx2DMappingContentShape>(obj)) {

            for (uint j = 0; j < shapes[i].src.size(); j++){
                if (shapes[i].src[j].bBeingDragged == true){
                    if(mouse.x < mapping_rect_src.x+mapping_rect_src.width) {
                        if(mouse.x > mapping_rect_src.x)
                            shapes[i].src[j].x = mouse.x;
                        else
                            shapes[i].src[j].x = mapping_rect_src.x;
                    }
                    else {
                        shapes[i].src[j].x = mapping_rect_src.x+mapping_rect_src.width;
                    }
                    if(mouse.y < mapping_rect_src.y+mapping_rect_src.height) {
                        if(mouse.y > mapping_rect_src.y)
                            shapes[i].src[j].y = mouse.y;
                        else
                            shapes[i].src[j].y = mapping_rect_src.y;
                    }
                    else {
                        shapes[i].src[j].y = mapping_rect_src.y+mapping_rect_src.height;
                    }
                    cshape->src[j].x = (shapes[i].src[j].x-mapping_rect_src.x)/mapping_rect_src.width;
                    cshape->src[j].y = (shapes[i].src[j].y-mapping_rect_src.y)/mapping_rect_src.height;

                    switch(j) {
                    case 0: {
                        cshape->src[1].y = cshape->src[j].y;
                        cshape->src[3].x = cshape->src[j].x;
                        shapes[i].src[1].y = shapes[i].src[j].y;
                        shapes[i].src[3].x = shapes[i].src[j].x;
                        break;
                    }
                    case 1: {
                        cshape->src[0].y = cshape->src[j].y;
                        cshape->src[2].x = cshape->src[j].x;
                        shapes[i].src[0].y = shapes[i].src[j].y;
                        shapes[i].src[2].x = shapes[i].src[j].x;
                        break;
                    }
                    case 2: {
                        cshape->src[1].x = cshape->src[j].x;
                        cshape->src[3].y = cshape->src[j].y;
                        shapes[i].src[1].x = shapes[i].src[j].x;
                        shapes[i].src[3].y = shapes[i].src[j].y;
                        break;
                    }
                    case 3: {
                        cshape->src[0].x = cshape->src[j].x;
                        cshape->src[2].y = cshape->src[j].y;
                        shapes[i].src[0].x = shapes[i].src[j].x;
                        shapes[i].src[2].y = shapes[i].src[j].y;
                        break;
                    }
                    default:
                        break;
                    }

                    cshape->newpos = true;
                }
            }

        }

    }

    if(dragging_dst) {
        zoom_point_offset += mouse - last_mouse;
        last_mouse = mouse;
    }

    return ofxPanel::mouseDragged(args);
}

bool ofx2DFormMapping::mousePressed(ofMouseEventArgs& args) {

    ofPoint mouse(args.x,args.y);

    bool on_element = false;

    for (uint i = 0; i < shapes.size(); i++){
        bool editable = parent_projector->getMappingObject(i)->editable;
        for (uint j = 0; j < shapes[i].polyline.size(); j++){
            ofPoint zoomed_p = addZoomRelativeOfDstRect(shapes[i].polyline[j]);
            float diffx = mouse.x - zoomed_p.x;
            float diffy = mouse.y - zoomed_p.y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < shapes[i].polyline[j].radius){
                if(editable) {
                    shapes[i].polyline[j].bBeingDragged = true;
                    on_element = true;
                }
            } else {
                shapes[i].polyline[j].bBeingDragged = false;
            }
        }
        for (uint j = 0; j < shapes[i].src.size(); j++){
            float diffx = mouse.x - shapes[i].src[j].x;
            float diffy = mouse.y - shapes[i].src[j].y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < shapes[i].src[j].radius){
                if(editable) {
                    shapes[i].src[j].bBeingDragged = true;
                }
            } else {
                shapes[i].src[j].bBeingDragged = false;
            }
        }
    }

    if(!on_element && mapping_rect_dst.inside(mouse)) {
        dragging_dst = true;
        last_mouse = mouse;
    }

    return ofxPanel::mousePressed(args);
}

bool ofx2DFormMapping::mouseReleased(ofMouseEventArgs &args) {

    for (uint i = 0; i < shapes.size(); i++){
        for (uint j = 0; j < shapes[i].polyline.size(); j++){
            shapes[i].polyline[j].bBeingDragged = false;
            if(shapes.size()-1-i < parent_list->getListItems().size() && parent_list->getListItems().size() > 0 && shapes.size() == parent_list->getListItems().size())
                parent_list->getListItems().at(shapes.size()-1-i)->setBackgroundColor(parent_list->getBackgroundColor());
        }
        for (uint j = 0; j < shapes[i].src.size(); j++){
            shapes[i].src[j].bBeingDragged = false;
        }
    }

    dragging_dst = false;

    return ofxPanel::mouseReleased(args);
}

bool ofx2DFormMapping::mouseScrolled(ofMouseEventArgs &args) {

    if(!direct_edit && mapping_rect_dst.inside(ofGetMouseX(), ofGetMouseY())) {
        setZoomFactor(args.y);
    }

    return ofxPanel::mouseScrolled(args);
}

void ofx2DFormMapping::setMappingBackground(ofFbo_ptr &fbo) {
    mapping_bg = fbo;
}

void ofx2DFormMapping::setEditMode(bool direct_edit) {
    this->direct_edit = direct_edit;
    if(direct_edit) {
        control_rect_backup = control_rect;
        setZoomFactor(0);
    }
    else {
        control_rect = control_rect_backup;
    }
    rebuild();
}

void ofx2DFormMapping::setOutputForm(float x, float y, float w, float h) {
    if(x != mapping_rect_output.x || y != mapping_rect_output.y || w != mapping_rect_output.width || h != mapping_rect_output.height) {
        mapping_rect_output = ofRectangle(x,y,w,h);
        rebuild();
    }
    else {
        mapping_rect_output = ofRectangle(x,y,w,h);
    }
}

void ofx2DFormMapping::setOutputForm(ofRectangle rect) {
    mapping_rect_output = rect;
}

void ofx2DFormMapping::setZoomFactor(int factor) {

    int old_zoom_factor = zoom_factor;

    zoom_factor += factor;
    if(zoom_factor < 0)
        zoom_factor = 0;

    ofPoint zoom_point_old = zoom_point;

    ofPoint tmp_zoom_point;
    tmp_zoom_point.x = ofGetMouseX() - mapping_rect_dst.x - zoom_point_offset.x;
    tmp_zoom_point.y = ofGetMouseY() - mapping_rect_dst.y - zoom_point_offset.y;

    ofVec2f diff = tmp_zoom_point - zoom_point_old;

    if(old_zoom_factor == 0) {
        diff = ofPoint(0,0);
        zoom_point_offset = ofPoint(0,0);
        zoom_point_old = tmp_zoom_point;
    }

    zoom_point = zoom_point_old + removeZoom(diff);
    zoom_point_offset += tmp_zoom_point - zoom_point;
    zoom_point_scaled = addZoom(zoom_point);

}

ofPoint ofx2DFormMapping::addZoom(ofPoint p) {
    return p*(1+zoom_factor*zoom_speed);
}

ofPoint ofx2DFormMapping::addZoomRelativeOfDstRect(ofPoint p) {
    return addZoom(p-mapping_rect_dst.getPosition())+mapping_rect_dst.getPosition()+translation_dst;
}

ofPoint ofx2DFormMapping::removeZoomRelativeOfDstRect(ofPoint p) {
    return removeZoom(p-mapping_rect_dst.getPosition()-translation_dst)+mapping_rect_dst.getPosition();
}

float ofx2DFormMapping::addZoom(float p) {
    return p*(1+zoom_factor*zoom_speed);
}


ofPoint ofx2DFormMapping::removeZoom(ofPoint p) {
    return p/(1+zoom_factor*zoom_speed);
}

void ofx2DFormMapping::setSize(float w, float h) {
    ofxPanel::setSize(w,h);
    control_rect = this->getShape();
    rebuild();
}
