#include "FormMapping.h"

FormMapping::FormMapping(): ofxPanel() {

    shapes.clear();

}

void FormMapping::setup(string title, Projector *parent_projector, ofxSortableList *parent_list, float w, float h) {
    ofxPanel::setup(title);
    setSize(w, h);
    control_rect = this->getShape();
    control_rect_backup = control_rect;
    this->parent_projector = parent_projector;
    this->parent_list = parent_list;
}

void FormMapping::setMappingRects() {

    int margin = 10;
    int header = 20;

    if(direct_edit) {
        mapping_rect_dst = mapping_rect_output;
        mapping_rect_src.x = control_rect.x+margin;
        mapping_rect_src.y = control_rect.y+margin+header;
        mapping_rect_src.width = control_rect.width/2;
        float content_ratio = parent_projector->outputHeight()/parent_projector->outputWidth();
        mapping_rect_src.height = mapping_rect_src.width*content_ratio;

        this->setSize(mapping_rect_src.width+2*margin, header+mapping_rect_src.height+2*margin);
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

        this->setSize(mapping_rect_dst.width+2*margin, mapping_rect_dst.height+header+mapping_rect_src.height+3*margin);
    }
}

void FormMapping::updateForms() {

    shapes.clear();

    for (uint i = 0; i < parent_projector->shapeCount(); i++) {

        MappingObject_ptr obj = parent_projector->getShape(i);

        if(MappingShape_ptr shape = std::dynamic_pointer_cast<MappingShape>(obj)) {

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

            if(MappingContentShape_ptr cshape = std::dynamic_pointer_cast<MappingContentShape>(shape)) {
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
            if(MappingPoint_ptr point = std::dynamic_pointer_cast<MappingPoint>(obj)) {

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

    //find first content shape from top to use as background for source mapping

    vector<MappingContentShape_ptr> objs = parent_projector->getShapesByClass<MappingContentShape>();
    if(objs.size() > 0) {
        source_bg = objs.at(objs.size()-1)->getTexture();
    }

}

void FormMapping::update() {


    if(control_rect.position != this->getPosition()) {
        control_rect.position = this->getPosition();
        control_rect.setWidth(this->getWidth());
        control_rect.setHeight(this->getHeight());

        rebuild();
    }

}

void FormMapping::rebuild() {
    setMappingRects();
    updateForms();
}

void FormMapping::draw(bool show_source) {

    ofSetColor(42);
    ofFill();
    ofDrawRectangle(this->getPosition().x, this->getPosition().y, this->getWidth(), this->getHeight()-2);

    ofxPanel::draw();

    if(!direct_edit) {
        ofSetColor(200);
        ofNoFill();
        ofSetLineWidth(0.5);
        ofDrawRectangle(mapping_rect_dst);
        ofSetColor(255,60);
        mapping_bg->draw(
                    mapping_rect_dst.x,
                    mapping_rect_dst.y,
                    mapping_rect_dst.width,
                    mapping_rect_dst.height);
    }

    if(show_source) {
        ofDrawRectangle(mapping_rect_src);
        if(source_bg) {
            ofSetColor(255,60);
            source_bg->draw(
                        mapping_rect_src.x,
                        mapping_rect_src.y,
                        mapping_rect_src.width,
                        mapping_rect_src.height);
        }
    }

    ofEnableAlphaBlending();

    ofSetLineWidth(1);

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
                    ofVertex(shapes[i].polyline[j].x, shapes[i].polyline[j].y);
                }
                ofEndShape(true);
            }
        }

        //draw src

        ofNoFill();
        ofSetColor(shapes[i].color,255);
        ofBeginShape();

        for(uint j = 0; j < shapes[i].src.size(); j++) {
            ofVertex(shapes[i].src[j].x, shapes[i].src[j].y);
        }
        ofEndShape(true);

        //draw dragging points
        ofSetColor(255,255,255,200);
        if(parent_projector->getShape(i)->editable) {
            for(uint j = 0; j < shapes[i].polyline.size(); j++) {
                if (shapes[i].polyline[j].bOver) ofFill();
                else ofNoFill();
                ofDrawCircle(shapes[i].polyline[j].x, shapes[i].polyline[j].y,6);

                if(j < shapes[i].src.size()) {
                    if (shapes[i].src[j].bOver) ofFill();
                    else ofNoFill();
                    if(j%2==0)
                        ofDrawCircle(shapes[i].src[j].x, shapes[i].src[j].y,6);
                }
            }
        }

    }

}

bool FormMapping::mouseMoved(ofMouseEventArgs& args) {

    ofPoint mouse(args.x,args.y);
    for (uint i = 0; i < shapes.size(); i++){
        for (uint j = 0; j < shapes[i].polyline.size(); j++){
            float diffx = mouse.x - shapes[i].polyline[j].x;
            float diffy = mouse.y - shapes[i].polyline[j].y;
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


bool FormMapping::mouseDragged(ofMouseEventArgs &args) {

    ofPoint mouse(args.x,args.y);

    for (uint i = 0; i < shapes.size(); i++) {

        MappingObject_ptr obj = parent_projector->getShape(i);

        if(MappingShape_ptr shape = std::dynamic_pointer_cast<MappingShape>(obj)) {

            for (uint j = 0; j < shapes[i].polyline.size(); j++) {

                if (shapes[i].polyline[j].bBeingDragged == true){
                    if(mouse.x < mapping_rect_dst.x+mapping_rect_dst.width) {
                        if(mouse.x > mapping_rect_dst.x)
                            shapes[i].polyline[j].x = mouse.x;
                        else
                            shapes[i].polyline[j].x = mapping_rect_dst.x;
                    }
                    else {
                        shapes[i].polyline[j].x = mapping_rect_dst.x+mapping_rect_dst.width;
                    }
                    if(mouse.y < mapping_rect_dst.y+mapping_rect_dst.height) {
                        if(mouse.y > mapping_rect_dst.y)
                            shapes[i].polyline[j].y = mouse.y;
                        else
                            shapes[i].polyline[j].y = mapping_rect_dst.y;
                    }
                    else {
                        shapes[i].polyline[j].y = mapping_rect_dst.y+mapping_rect_dst.height;
                    }

                    if(shapes[i].polyline.size() == 4) {

                        shape->dst[j].x = (shapes[i].polyline[j].x-mapping_rect_dst.x)/mapping_rect_dst.width;
                        shape->dst[j].y = (shapes[i].polyline[j].y-mapping_rect_dst.y)/mapping_rect_dst.height;

                        shape->polyline[j].x = shape->dst[j].x;
                        shape->polyline[j].y = shape->dst[j].y;

                    }
                    else {

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

                    shape->newpos = true;
                    parent_list->getListItems().at(shapes.size()-1-i)->setBackgroundColor(ofColor(0,200,210));
//                    Visuals::get().reloadLinesFromRaw();
                }
            }

            if(MappingContentShape_ptr cshape = std::dynamic_pointer_cast<MappingContentShape>(obj)) {

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

    }

    return ofxPanel::mouseDragged(args);
}

bool FormMapping::mousePressed(ofMouseEventArgs& args) {

    ofPoint mouse(args.x,args.y);

    for (uint i = 0; i < shapes.size(); i++){
        bool editable = parent_projector->getShape(i)->editable;
        for (uint j = 0; j < shapes[i].polyline.size(); j++){
            float diffx = mouse.x - shapes[i].polyline[j].x;
            float diffy = mouse.y - shapes[i].polyline[j].y;
            float dist = sqrt(diffx*diffx + diffy*diffy);
            if (dist < shapes[i].polyline[j].radius){
                if(editable) {
                    shapes[i].polyline[j].bBeingDragged = true;
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

    return ofxPanel::mousePressed(args);
}

bool FormMapping::mouseReleased(ofMouseEventArgs &args) {

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

    return ofxPanel::mouseReleased(args);
}

void FormMapping::setMappingBackground(ofFbo_ptr &fbo) {
    mapping_bg = fbo;
}

void FormMapping::setEditMode(bool direct_edit) {
    this->direct_edit = direct_edit;
    if(direct_edit) {
        control_rect_backup = control_rect;
    }
    else {
        control_rect = control_rect_backup;
    }
    rebuild();
}

void FormMapping::setOutputForm(float x, float y, float w, float h) {
    mapping_rect_output = ofRectangle(x,y,w,h);
}

void FormMapping::setOutputForm(ofRectangle rect) {
    mapping_rect_output = rect;
}
