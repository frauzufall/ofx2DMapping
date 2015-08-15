#include "ofx2DMappingController.h"
#include "ofx2DMappingObject.h"

ofx2DMappingController::ofx2DMappingController() {

    xml_mapping = "mapping/mapping.xml";
    svg_mapping = "mapping/mapping.svg";
    png_mapping = "mapping/mapping.png";

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

    is_cal.set("calibration", false);
    cal_border.set("line width", 1, 1, 7);
    cal_grey.set("lightness", 100, 0, 255);

    controlpoint = ofPoint(20,20);

    available_shapes.clear();

    output_rectangle.x = 0;
    output_rectangle.y = 0;
    output_rectangle.width = ofGetWindowWidth();
    output_rectangle.height = ofGetWindowHeight();

}

ofx2DMappingController::~ofx2DMappingController(){
//    getProjector()->outputWidth().removeListener(this, &ofx2DMappingController::outputSizeChanged);
//    getProjector()->outputHeight().removeListener(this, &ofx2DMappingController::outputSizeChanged);
}

void ofx2DMappingController::setup(string xml_path) {

    ofRegisterKeyEvents(this);

    xml_mapping = xml_path;
    ofFile xml = ofFile(xml_path);
    svg_mapping = xml.getEnclosingDirectory() + xml.getBaseName() + ".svg";
    png_mapping = xml.getEnclosingDirectory() + xml.getBaseName() + ".png";

    setupMapping();

    output_rectangle.setWidth(getProjector()->outputWidth());
    output_rectangle.setHeight(getProjector()->outputHeight());
//    getProjector()->outputWidth().addListener(this, &ofx2DMappingController::outputSizeChanged);
//    getProjector()->outputHeight().addListener(this, &ofx2DMappingController::outputSizeChanged);

    mapped_content_fbo = ofFbo_ptr(new ofFbo());
    mapped_content_fbo->allocate(getProjector()->outputWidth(), getProjector()->outputHeight(), GL_RGBA);

    mapped_area_fbo = ofFbo_ptr(new ofFbo());
    mapped_area_fbo->allocate(getProjector()->outputWidth(), getProjector()->outputHeight(), GL_RGBA);

}

void ofx2DMappingController::setupMapping(){
    ofxXmlSettings_ptr xml = ofxXmlSettings_ptr(new ofxXmlSettings());
    xml->clear();
    if(!xml->loadFile(xml_mapping) ) {
        ofFile newFile(xml_mapping);
        newFile.create();
        ofLogNotice("ofx2DMappingController: setupMapping()", "unable to load xml file " + xml_mapping + ", creating empty stage.");
    }
    reloadMapping(xml);

}

void ofx2DMappingController::reloadMapping(ofxXmlSettings_ptr xml) {

    if(xml->tagExists("mapping")) {
        use_mapping = xml->getAttribute("mapping", "active", (int)true);

        xml->pushTag("mapping", 0);

        xml->pushTag("control", 0);

        control_w.set(xml->getValue("width", 1024.));
        control_h.set(xml->getValue("height", 768.));
        if(control_w <= 0){
            control_w = 1024;
        }
        if(control_h <= 0){
            control_h = 768;
        }

        xml->popTag();

        int projector_count = xml->getNumTags("projector");

        for(int i = 0; i < projector_count; i++) {

            xml->pushTag("projector", i);

                xml->pushTag("output", 0);

                    float output_w		= xml->getValue("width", 1600.);
                    float output_h		= xml->getValue("height", 900.);
                    if(output_w <= 0){
                        output_w = 1600;
                    }
                    if(output_h <= 0){
                        output_h = 900;
                    }

                xml->popTag();

                addProjector(output_w, output_h);

                int object_count = xml->getNumTags("object");

                for (int j = 0; j < object_count; j++) {

                    string type = xml->getAttribute("object","type","OBJECT",j);

                    string name = xml->getAttribute("object","name","content",j);

                    xml->pushTag("object", j);

                        ofPtr<ofx2DMappingObject> obj = createShape(getProjector(i), type, name);
                        if(obj) {
                            obj->loadXml(xml);
                        }
                        else {
                            ofLogError("ofx2DMappingController::reloadMapping()", "Could not load mapping object with type " + type + " from xml");
                        }

                    xml->popTag();

                }

            xml->popTag();

            ofLogNotice("ofx2DMappingController: reloadMapping()", "projector " + ofToString(i) + " with " + ofToString(getProjector(i)->shapeCount()) + " mapping objects loaded.");

        }
    }

    if(projectors.size() == 0) {
        addProjector(1600, 900);
    }

}

ofPtr<ofx2DMappingObject> ofx2DMappingController::createShape(ofx2DMappingProjector* projector, string type, string name) {

    ofPtr<ofx2DMappingObject> res;

    uint i = 0;
    for(; i < available_shapes.size(); i++) {
        if(available_shapes.at(i)->name == name) {
            res = projector->copyShape(available_shapes.at(i));
            break;
        }
    }
    if(i == available_shapes.size()) {
        res = projector->addShape(type, name);
    }

    return res;

}

void ofx2DMappingController::update() {

    //check if any object template wants to be copied to stage
    for(auto &e: getOptions()){
        if(e->pleaseCopyMe.get()){
            e->pleaseCopyMe.set(false);
            getProjector()->copyShape(e);
        }
    }

    for(uint i = 0; i < projectors.size(); i++) {
        projectors[i].update();
    }

    updateFbo(0);
    updateAreaFbo(0);

}

void ofx2DMappingController::updateFbo(int projector_id) {

    ofEnableAlphaBlending();
    glEnable (GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);

    if(projector_id < (int)projectors.size()) {

        ofx2DMappingProjector* p = &(projectors[projector_id]);

        mappedContentToFbo(p);

    }
    else {
        ofLogError("ofx2DMappingController: updateFbo()", "trying to draw projector " + ofToString(projector_id) + " but projectors size is " + ofToString(projectors.size()));
    }
}

void ofx2DMappingController::updateAreaFbo(int projector_id) {

    ofEnableAlphaBlending();
    glEnable (GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);

    if(projector_id < (int)projectors.size()) {

        ofx2DMappingProjector* p = &(projectors[projector_id]);

        mappedAreaToFbo(p);

    }
    else {
        ofLogError("ofx2DMappingController: updateAreaFbo()", "trying to draw projector " + ofToString(projector_id) + " but projectors size is " + ofToString(projectors.size()));
    }
}


void ofx2DMappingController::mappedContentToFbo(ofx2DMappingProjector *p) {

    ofEnableAlphaBlending();
    glEnable (GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_LINE_SMOOTH, GL_NICEST);

    mapped_content_fbo->begin();
    ofClear(0, 0, 0, 255);

    for(uint i = 0; i < p->shapeCount(); i++) {

        ofPtr<ofx2DMappingObject> q = p->getMappingObject(i);
        q->draw(p->outputWidth(), p->outputHeight());
    }

    if(getCalibrating()) drawCalibration(p);

    mapped_content_fbo->end();

}

void ofx2DMappingController::mappedAreaToFbo(ofx2DMappingProjector *p) {

    mapped_area_fbo->begin();
    ofClear(0, 0, 0, 255);

    for(uint i = 0; i < p->shapeCount(); i++) {

        p->getMappingObject(i)->drawArea(p->outputWidth(), p->outputHeight());
    }

    mapped_area_fbo->end();

}

void ofx2DMappingController::drawCalibration(ofx2DMappingProjector* p) {

    ofEnableAlphaBlending();

    for(uint i = 0; i < p->shapeCount(); i++) {

       ofPtr<ofx2DMappingObject> q = p->getMappingObject(i);

        if(q) {

//            ofSetColor(q->color);
//            ofFill();

//            ofBeginShape();
//            for(int j= 0; j < p->outlinesRaw()->at(i).getVertices().size(); j++) {
//                ofVertex(p->outlinesRaw()->at(i).getVertices().at(j).x, p->outlinesRaw()->at(i).getVertices().at(j).y);
//            }
//            ofEndShape();

            ofSetColor(cal_grey);
            ofNoFill();
            ofSetLineWidth(cal_border);

            p->outlinesRaw()->at(i).draw();

        }
    }

}

ofParameter<bool>& ofx2DMappingController::getCalibrating() {
    return is_cal;
}

ofParameter<float>& ofx2DMappingController::getCalBorder() {
    return cal_border;
}

ofParameter<int>& ofx2DMappingController::getCalGrey() {
    return cal_grey;
}

void ofx2DMappingController::addProjector(float w, float h) {

    projectors.push_back(ofx2DMappingProjector(w,h));

}

ofx2DMappingProjector* ofx2DMappingController::getProjector(int id) {
    if(id < (int)projectors.size()) {
        return &(projectors[id]);
    }
    else {
        ofLogError("ofx2DMappingController: getProjector()", "trying to get projector " + ofToString(id) + " but projectors size is " + ofToString(projectors.size()));
        return 0;
    }
}

ofPoint ofx2DMappingController::getPointInMappedArea(ofPoint last_p, ofPoint next_p) {

    ofx2DMappingProjector *p = getProjector();
    ofPoint last_p_norm(last_p.x/p->outputWidth(), last_p.y/p->outputHeight());
    ofPoint res_norm;
    ofPoint res = next_p;
    //TODO rewrite to fit new model
    if(next_p.x < 0 || next_p.y < 0)
        res = last_p;
    if(next_p.x > p->outputWidth() || next_p.y > p->outputHeight())
        res = last_p;

    vector<ofPtr<ofx2DMappingShape>> windows_next_inside;
    vector<ofPtr<ofx2DMappingShape>> paintings_next_inside;
    vector<ofPtr<ofx2DMappingShape>> paintings_last_inside;

    windows_next_inside.clear();
    paintings_next_inside.clear();
    paintings_last_inside.clear();

    res_norm.x = res.x/p->outputWidth();
    res_norm.y = res.y/p->outputHeight();

    for(uint i = 0; i < getProjector()->shapeCount(); i++) {

        ofPtr<ofx2DMappingShape> shape = std::dynamic_pointer_cast<ofx2DMappingShape>(getProjector()->getMappingObject(i));

        if(shape) {

            vector<ofPoint> polyline = shape->polyline.getVertices();

            if(shape->triangle.isPointInsidePolygon(&polyline[0],polyline.size(),res_norm)) {

                if(shape->nature == "window")
                    windows_next_inside.push_back(shape);

                if(shape->nature == "drawing area")
                    paintings_next_inside.push_back(shape);
            }

            if(shape->nature == "drawing area") {
                if(shape->triangle.isPointInsidePolygon(&polyline[0],polyline.size(),last_p_norm)) {
                    paintings_last_inside.push_back(shape);
                }
            }
        }
    }

    if(paintings_next_inside.size() == 0) {

        //next point is outside of mapped area
        //find point between last and next point on the edge of the mapped area

        //1. find intersections between last and next point on the paintings the last point is in
        //2. chose intersection that is nearest to the next point

        float dist_res_nextp = 1000000;

        if(paintings_last_inside.size() == 0) {
            cout << "next and last point are in no paintings" << endl;
            vector<ofPtr<ofx2DMappingPoint>> pts = getProjector()->getShapesByClass<ofx2DMappingPoint>();
            if(pts.size() > 0) {
                res = getProjector()->getShapesByClass<ofx2DMappingPoint>()[0]->pos;
            }
            else {
                res.x = p->outputWidth()/2;
                res.y = p->outputHeight()/2;
            }
        }

        for(uint i = 0; i < paintings_last_inside.size(); i++) {

            ofPtr<ofx2DMappingShape> shape = paintings_last_inside.at(i);

            res_norm.x = res.x/p->outputWidth();
            res_norm.y = res.y/p->outputHeight();

            ofPoint corrected_norm = intersectionPointPolyline(last_p_norm, res_norm, shape->polyline);
            ofPoint corrected(corrected_norm.x*p->outputWidth(), corrected_norm.y*p->outputHeight());
            float dist = ofDist(next_p.x, next_p.y, corrected.x, corrected.y);
            if(dist<dist_res_nextp) {
                dist_res_nextp = dist;
                res = corrected;
            }

        }


    }
    if(windows_next_inside.size()>0) {
        //next point is in a window area where it is not allowed
        //find point between last and next point on the edge of the window

        //1. find intersections between last and next point windows the next point is in
        //2. chose intersection that is nearest to the next point

        float dist_res_nextp = 1000000;

        for(uint i = 0; i < windows_next_inside.size(); i++) {

            ofPtr<ofx2DMappingShape> shape = windows_next_inside.at(i);

            res_norm.x = res.x/p->outputWidth();
            res_norm.y = res.y/p->outputHeight();

            ofPoint corrected_norm = intersectionPointPolyline(last_p_norm, res_norm, shape->polyline);
            ofPoint corrected(corrected_norm.x*p->outputWidth(), corrected_norm.y*p->outputHeight());
            float dist = ofDist(next_p.x, next_p.y, corrected.x, corrected.y);
            if(dist<dist_res_nextp) {
                dist_res_nextp = dist;
                res = corrected;
            }

        }
    }

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
                intersections.push_back(intersection-last_inter_norm*1/getProjector()->outputWidth());
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

ofFbo_ptr &ofx2DMappingController::getOutput() {
    return mapped_content_fbo;
}

ofFbo_ptr ofx2DMappingController::getArea() {
    return mapped_area_fbo;
}

void ofx2DMappingController::saveOutputImage() {
    stringstream str;
    str << "screenshots/";
    str << ofGetTimestampString() << ".png";
    ofLogNotice("ofx2DMappingController: saveOutputImage()", "saving image " + str.str());
    ofImage img;
    img.allocate(getOutput()->getWidth(), getOutput()->getHeight(), OF_IMAGE_COLOR_ALPHA);
    getOutput()->readToPixels(img.getPixels());
    img.update();
    img.save(str.str());
}

void ofx2DMappingController::saveMappingDefault() {
    saveMapping(xml_mapping, svg_mapping, png_mapping);
}

void ofx2DMappingController::saveMapping(string path, string path_svg, string path_png) {

    ofxXmlSettings_ptr xml = ofxXmlSettings_ptr(new ofxXmlSettings());

    xml->clear();

    xml->addTag("mapping");

    xml->pushTag("mapping", 0);

        xml->addTag("control");
        xml->pushTag("control", 0);
            xml->addValue("width", (int)control_w);
            xml->addValue("height", (int)control_h);
        xml->popTag();

        xml->addTag("projector");

        xml->pushTag("projector", 0);

            int i = 0;

            xml->addTag("output");
            xml->pushTag("output", 0);
                xml->addValue("width", (int)getProjector()->outputWidth());
                xml->addValue("height", (int)getProjector()->outputHeight());
            xml->popTag();

            for(uint j = 0; j < getProjector()->shapeCount(); j++) {

                ofPtr<ofx2DMappingObject> mq = getProjector()->getMappingObject(j);

                if(mq) {

                    xml->addTag("object");

                    xml->addAttribute("object","type",mq->nature, i);
                    xml->addAttribute("object","name",mq->name, i);
                    xml->pushTag("object", i);

                        mq->saveXml(xml);

                    xml->popTag();

                    i++;

                }
            }

        xml->popTag();

    xml->popTag();

    xml->saveFile(path);

    saveMappingAsPng(path_png);

    getProjector()->exportSvg(path_svg);

}

void ofx2DMappingController::saveMappingAsPng() {
    saveMappingAsPng(png_mapping);
}

void ofx2DMappingController::saveMappingAsPng(string path) {

    ofFbo_ptr fbo = getArea();
    ofImage img;
    unsigned char* pixels = new unsigned char[(int)getProjector()->outputWidth()*(int)getProjector()->outputHeight()*4];
    img.allocate(fbo->getWidth(), fbo->getHeight(), OF_IMAGE_COLOR_ALPHA);
    img.setUseTexture(false);
    fbo->begin();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, fbo->getWidth(), fbo->getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    img.setFromPixels(pixels, fbo->getWidth(), fbo->getHeight(), OF_IMAGE_COLOR_ALPHA);
    fbo->end();

    img.save(path);

}

void ofx2DMappingController::saveMappingAsSvg() {
    getProjector()->exportSvg(svg_mapping);
}

void ofx2DMappingController::importSvg(const std::string path) {
    getProjector()->importSvg(path);
}

void ofx2DMappingController::importSvg() {
    getProjector()->importSvg(svg_mapping);
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

ofParameter<float> &ofx2DMappingController::controlWidth() {
    return control_w;
}

ofParameter<float> &ofx2DMappingController::controlHeight() {
    return control_h;
}

void ofx2DMappingController::addTemplate(ofPtr<ofx2DMappingObject> obj) {
    available_shapes.push_back(obj);
}

vector<ofPtr<ofx2DMappingObject>> ofx2DMappingController::getOptions() {
    return available_shapes;
}

ofRectangle ofx2DMappingController::getOutputShape() {
    return output_rectangle;
}

void ofx2DMappingController::setOutputShape(ofRectangle r){
    output_rectangle = r;
    mapped_content_fbo->clear();
    mapped_area_fbo->clear();
    mapped_content_fbo->allocate(r.width, r.height, GL_RGBA);
    mapped_area_fbo->allocate(r.width, r.height, GL_RGBA);
    getProjector()->setOutputSize(r.width,r.height);
//    getProjector()->outputWidth().set(r.width);
//    getProjector()->outputHeight().set(r.height);
}

void ofx2DMappingController::setOutputPosition(float x, float y){
    output_rectangle.x = x;
    output_rectangle.y = y;
}

void ofx2DMappingController::outputSizeChanged(){
    mapped_content_fbo->clear();
    mapped_area_fbo->clear();
    mapped_content_fbo->allocate(getProjector()->outputWidth(), getProjector()->outputHeight(), GL_RGBA);
    mapped_area_fbo->allocate(getProjector()->outputWidth(), getProjector()->outputHeight(), GL_RGBA);
    output_rectangle.setWidth(getProjector()->outputWidth());
    output_rectangle.setHeight(getProjector()->outputHeight());
}
