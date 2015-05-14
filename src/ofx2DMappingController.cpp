#include "ofx2DMappingController.h"
#include "MappingObject.h"

ofx2DMappingController::ofx2DMappingController() {

    xml_mapping = "mapping/mapping.xml";

    projectors.clear();

    //we set matrix to the default - 0 translation
    //and 1.0 scale for x y z and w
    for(int i = 0; i < 16; i++)
    {
        if(i % 5 != 0) matrix[i] = 0.0;
        else matrix[i] = 1.0;
    }
	
	plane[0] = ofPoint(0, 0, 0);
    plane[1] = ofPoint(1, 0, 0);
    plane[2] = ofPoint(1, 1, 0);
    plane[3] = ofPoint(0, 1, 0);

    is_cal = false;
    cal_border = 0.05;
    cal_grey = 100;

    controlpoint = ofPoint(20,20);

    ofRegisterKeyEvents(this);

}

void ofx2DMappingController::setup(string xml_path, float width, float height) {

    xml_mapping = xml_path;

    output_w = width;
    output_h = height;

    setupMapping();

    mapped_content_fbo = ofFbo_ptr(new ofFbo());
    mapped_content_fbo->allocate(output_w, output_h, GL_RGBA);

    mapped_area_fbo = ofFbo_ptr(new ofFbo());
    mapped_area_fbo->allocate(output_w, output_h, GL_RGBA);

}

void ofx2DMappingController::setupMapping(){
    ofxXmlSettings_ptr xml = ofxXmlSettings_ptr(new ofxXmlSettings());
    xml->clear();
    if( xml->loadFile(xml_mapping) ){
        reloadMapping(xml);
    }else{
        cout << "unable to load xml file " << xml_mapping << endl;
    }

}

void ofx2DMappingController::reloadMapping(ofxXmlSettings_ptr xml) {

    use_mapping = xml->getAttribute("mapping", "active", (int)true);

    xml->pushTag("mapping", 0);

    xml->pushTag("content", 0);

    content_w		= xml->getValue("width", 640.);
    content_h		= xml->getValue("height", 480.);

    xml->popTag();
    xml->pushTag("output", 0);

    output_w		= xml->getValue("width", 1024.);
    output_h		= xml->getValue("height", 768.);

    xml->popTag();
    xml->pushTag("control", 0);

    control_w		= xml->getValue("width", 1024.);
    control_h		= xml->getValue("height", 768.);

    xml->popTag();
    xml->pushTag("video", 0);

    vid_max_w		= xml->getValue("max_width", 640.);
    vid_max_h		= xml->getValue("max_height", 480.);

    xml->popTag();

    int projector_count = xml->getNumTags("projector");

    for(int i = 0; i < projector_count; i++) {

        xml->pushTag("projector", i);

        addProjector(output_w, output_h);

        int quad_count = xml->getNumTags("quad");

        for (int j = 0; j < quad_count; j++){

            string type = xml->getAttribute("quad","type","OBJECT",j);

            xml->pushTag("quad", j);

                MappingObject_ptr obj = getProjector(i)->addShape(type);
                obj->loadXml(xml);

                if(MappingContentShape_ptr shape = std::static_pointer_cast<MappingContentShape>(obj)) {
                    ofAddListener(ofx2DMappingController::updatedFbo, shape.get(), &MappingContentShape::updateFbo);
                }


            xml->popTag();

        }

        getProjector(i)->updateOutlines();

        xml->popTag();

        cout << "OFX2DMAPPINGCONTROLLER:: projector " << i << " with " << getProjector(i)->shapeCount() << " mapping objects loaded." << endl;

    }

}

void ofx2DMappingController::update() {

    for(uint i = 0; i < projectors.size(); i++) {
        projectors[i].update();
    }

}

void ofx2DMappingController::updateFbo(int projector_id) {

    ofEnableAlphaBlending();
    glEnable (GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);

    if(projector_id < (int)projectors.size()) {

        Projector* p = &(projectors[projector_id]);

        mappedContentToFbo(p);

    }
    else {
        cout << "ERROR: ofx2DMappingController: trying to draw projector " << projector_id << " but projectors size is " << projectors.size() << endl;
    }
}

void ofx2DMappingController::updateAreaFbo(int projector_id) {

    ofEnableAlphaBlending();
    glEnable (GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);

    if(projector_id < (int)projectors.size()) {

        Projector* p = &(projectors[projector_id]);

        mappedAreaToFbo(p);

    }
    else {
        cout << "ERROR: ofx2DMappingController: trying to draw projector " << projector_id << " but projectors size is " << projectors.size() << endl;
    }
}


void ofx2DMappingController::mappedContentToFbo(Projector *p) {

    ofEnableAlphaBlending();
    glEnable (GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);

    mapped_content_fbo->begin();
    ofClear(0, 0, 0, 255);

    for(uint i = 0; i < p->shapeCount(); i++) {

        MappingObject_ptr q = p->getShape(i);
        q->draw(output_w, output_h);
    }

    mapped_content_fbo->end();

}

void ofx2DMappingController::mappedAreaToFbo(Projector *p) {

    mapped_area_fbo->begin();
    ofClear(0, 0, 0, 255);

    for(uint i = 0; i < p->shapeCount(); i++) {

        p->getShape(i)->drawArea(output_w, output_h);
    }

    mapped_area_fbo->end();

}

void ofx2DMappingController::setCalibrating(bool calibrate) {
    is_cal = calibrate;
}

void ofx2DMappingController::drawCalibration(Projector* p) {

    ofEnableAlphaBlending();

    for(uint i = 0; i < p->shapeCount(); i++) {

       MappingObject_ptr q = p->getShape(i);

        if(q) {

            ofSetColor(q->color);
            ofFill();

            p->outlinesRaw()->at(i).draw();

            ofSetColor(cal_grey);
            ofNoFill();
            ofSetLineWidth(cal_border);

            p->outlinesRaw()->at(i).draw();

        }
    }

}

bool ofx2DMappingController::isCalibrating() {
    return is_cal;
}

void ofx2DMappingController::setCalBorder(float border) {
    cal_border = border;
}

void ofx2DMappingController::setCalGrey(int grey) {
    cal_grey = grey;
}

float ofx2DMappingController::getCalBorder() {
    return cal_border;
}

int ofx2DMappingController::getCalGrey() {
    return cal_grey;
}

void ofx2DMappingController::addProjector(float w, float h) {

    projectors.push_back(Projector(w,h));

}

Projector* ofx2DMappingController::getProjector(int id) {
    if(id < (int)projectors.size()) {
        return &(projectors[id]);
    }
    else {
        cout << "ERROR: ofx2DMappingController: trying to get projector " << id << " but projectors size is " << projectors.size() << endl;
        return 0;
    }
}

ofPoint ofx2DMappingController::getPointInMappedArea(ofPoint last_p, ofPoint next_p) {

    ofPoint last_p_norm(last_p.x/output_w, last_p.y/output_h);
    ofPoint res_norm;
    ofPoint res = next_p;
    //TODO rewrite to fit new model
//    if(next_p.x < 0 || next_p.y < 0)
//        res = last_p;
//    if(next_p.x > output_w || next_p.y > output_h)
//        res = last_p;

//    vector<MappingShape_ptr> windows_next_inside;
//    vector<MappingShape_ptr> paintings_next_inside;
//    vector<MappingShape_ptr> paintings_last_inside;

//    windows_next_inside.clear();
//    paintings_next_inside.clear();
//    paintings_last_inside.clear();

//    res_norm.x = res.x/output_w;
//    res_norm.y = res.y/output_h;

//    for(uint i = 0; i < getProjector(0)->shapeCount(); i++) {

//        MappingShape_ptr shape = std::static_pointer_cast<MappingShape>(getProjector(0)->getShape(i));

//        if(shape) {

//            vector<ofPoint> polyline = shape->polyline.getVertices();

//            if(shape->triangle.isPointInsidePolygon(&polyline[0],polyline.size(),res_norm)) {

//                if(shape->nature == COLOR_SHAPE)
//                    windows_next_inside.push_back(shape);

//                if(shape->nature == CONTENT_SHAPE)
//                    paintings_next_inside.push_back(shape);
//            }

//            if(shape->nature == CONTENT_SHAPE) {
//                if(shape->triangle.isPointInsidePolygon(&polyline[0],polyline.size(),last_p_norm)) {
//                    paintings_last_inside.push_back(shape);
//                }
//            }
//        }
//    }

//    if(paintings_next_inside.size() == 0) {

//        //next point is outside of mapped area
//        //find point between last and next point on the edge of the mapped area

//        //1. find intersections between last and next point on the paintings the last point is in
//        //2. chose intersection that is nearest to the next point

//        float dist_res_nextp = 1000000;

//        if(paintings_last_inside.size() == 0) {
//            cout << "next and last point are in no paintings" << endl;
//            vector<MappingPoint_ptr> pts = getProjector(0)->getShapesByClass<MappingPoint>();
//            if(pts.size() > 0) {
//                res = getProjector(0)->getShapesByClass<MappingPoint>()[0]->pos;
//            }
//            else {
//                res.x = output_w/2;
//                res.y = output_h/2;
//            }
//        }

//        for(uint i = 0; i < paintings_last_inside.size(); i++) {

//            MappingShape_ptr shape = paintings_last_inside.at(i);

//            res_norm.x = res.x/output_w;
//            res_norm.y = res.y/output_h;

//            ofPoint corrected_norm = intersectionPointPolyline(last_p_norm, res_norm, shape->polyline);
//            ofPoint corrected(corrected_norm.x*output_w, corrected_norm.y*output_h);
//            float dist = ofDist(next_p.x, next_p.y, corrected.x, corrected.y);
//            if(dist<dist_res_nextp) {
//                dist_res_nextp = dist;
//                res = corrected;
//            }

//        }


//    }
//    if(windows_next_inside.size()>0) {
//        //next point is in a window area where it is not allowed
//        //find point between last and next point on the edge of the window

//        //1. find intersections between last and next point windows the next point is in
//        //2. chose intersection that is nearest to the next point

//        float dist_res_nextp = 1000000;

//        for(uint i = 0; i < windows_next_inside.size(); i++) {

//            MappingShape_ptr shape = windows_next_inside.at(i);

//            res_norm.x = res.x/output_w;
//            res_norm.y = res.y/output_h;

//            ofPoint corrected_norm = intersectionPointPolyline(last_p_norm, res_norm, shape->polyline);
//            ofPoint corrected(corrected_norm.x*output_w, corrected_norm.y*output_h);
//            float dist = ofDist(next_p.x, next_p.y, corrected.x, corrected.y);
//            if(dist<dist_res_nextp) {
//                dist_res_nextp = dist;
//                res = corrected;
//            }

//        }
//    }

    //TODO: check that there is only mapped area between last and next point when they are not in the same polyline

    return res;
}

ofPoint ofx2DMappingController::getIntersection(ofPoint p1, ofPoint p2, ofPoint p3, ofPoint p4) {
    ofPoint p(0,0);
    float denom = (p1.x-p2.x)*(p3.y-p4.y) - (p1.y-p2.y)*(p3.x-p4.x);
    if(denom != 0) {
        p.x = ((p1.x*p2.y-p1.y*p2.x)*(p3.x-p4.x)-(p3.x*p4.y-p3.y*p4.x)*(p1.x-p2.x))/denom;
        p.y = ((p1.x*p2.y-p1.y*p2.x)*(p3.y-p4.y)-(p3.x*p4.y-p3.y*p4.x)*(p1.y-p2.y))/denom;
    }
    return p;
}

bool ofx2DMappingController::isLeft(ofPoint p1, ofPoint p2, ofPoint p_test){
     return ((p2.x - p1.x)*(p_test.y - p1.y) - (p2.y - p1.y)*(p_test.x - p1.x)) > 0;
}

bool ofx2DMappingController::isOnLine(ofPoint p1, ofPoint p2, ofPoint p_test){
    return ((p2.x - p1.x)*(p_test.y - p1.y) - (p2.y - p1.y)*(p_test.x - p1.x)) < 1;
}

ofPoint ofx2DMappingController::intersectionPointPolyline(ofPoint last_p, ofPoint next_p, ofPolyline polyline) {

    uint i, j=polyline.size()-1;

    ofPoint poly[polyline.size()];

    for (i=0; i<polyline.size(); i++) {
        poly[i] = polyline.getVertices().at(i);
    }

    vector<ofPoint> intersections;
    intersections.clear();

    for (i=0; i<polyline.size(); i++) {

        if(isLeft(poly[i],poly[j], last_p) != isLeft(poly[i],poly[j], next_p)
                && isLeft(next_p,last_p, poly[i]) != isLeft(next_p, last_p, poly[j])) {
            ofPoint intersection = getIntersection(last_p, next_p, poly[i], poly[j]);
            ofPoint last_inter_norm = (intersection-last_p).normalize();
            if(last_p.x<1)
                intersections.push_back(intersection-last_inter_norm*1/output_w);
            else
                intersections.push_back(intersection-last_inter_norm);
        }
        j=i;
    }

    ofPoint min_dif_p = next_p;
    float min_dif = 100000;
    for(uint i = 0; i<intersections.size(); i++) {
        float dist = last_p.distance(intersections.at(i));
        if(dist < min_dif) {
            min_dif_p = intersections.at(i);
            min_dif = dist;
        }
    }

    return min_dif_p;
}

ofFbo_ptr ofx2DMappingController::getOutput() {
    return mapped_content_fbo;
}

ofFbo_ptr ofx2DMappingController::getArea() {
    return mapped_area_fbo;
}

void ofx2DMappingController::saveOutputImage() {
    stringstream str;
    str << "screenshots/";
    str << ofGetTimestampString() << ".png";
    cout << "MAPPINGCONTROLLER::saving image " << str.str() << endl;
    ofImage img;
    img.allocate(mapped_content_fbo->getWidth(), mapped_content_fbo->getHeight(), OF_IMAGE_COLOR_ALPHA);
    mapped_content_fbo->readToPixels(img.getPixels());
    img.update();
    img.save(str.str());
}

void ofx2DMappingController::keyPressed(ofKeyEventArgs &args){

    switch(args.key) {
    case 'p':
        saveOutputImage();
        break;
    default:break;
    }

}

void ofx2DMappingController::keyReleased(ofKeyEventArgs &args){

}

void ofx2DMappingController::setInputFbo(ofFbo_ptr fbo) {
    src_fbo = fbo;
    ofNotifyEvent(ofx2DMappingController::updatedFbo, fbo);
}

float ofx2DMappingController::contentWidth() {
    return content_w;
}

float ofx2DMappingController::contentHeight() {
    return content_h;
}

float ofx2DMappingController::outputWidth() {
    return output_w;
}

float ofx2DMappingController::outputHeight() {
    return output_h;
}

float ofx2DMappingController::controlWidth() {
    return control_w;
}

float ofx2DMappingController::controlHeight() {
    return control_h;
}

float ofx2DMappingController::vidMaxWidth() {
    return vid_max_w;
}

float ofx2DMappingController::vidMaxHeight() {
    return vid_max_h;
}

void ofx2DMappingController::setOutputWidth(float val) {
    output_w = val;
}

void ofx2DMappingController::setOutputHeight(float val) {
    output_h = val;
}

void ofx2DMappingController::setControlWidth(float val) {
    control_w = val;
}

void ofx2DMappingController::setControlHeight(float val) {
    control_h = val;
}
