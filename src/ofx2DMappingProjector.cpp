#include "ofx2DMappingProjector.h"
#include "ofx2DMappingHelper.h"
#include "ofx2DMappingObjectFactory.h"
	
ofx2DMappingProjector::ofx2DMappingProjector(float w, float h) {
    shapes.clear();
    shapes.resize(0);
    plane[0] = ofPoint(0, 0, 0);
    plane[1] = ofPoint(1, 0, 0);
    plane[2] = ofPoint(1, 1, 0);
    plane[3] = ofPoint(0, 1, 0);

    camera.resize(4);
    camera[0] = ofPoint(0, 0, 0);
    camera[1] = ofPoint(1, 0, 0);
    camera[2] = ofPoint(1, 1, 0);
    camera[3] = ofPoint(0, 1, 0);
    camera_homography.makeIdentityMatrix();

    _svg = ofPtr<ofxSVG>(new ofxSVG());
    _outlines = ofPolylines_ptr(new vector<ofPolyline>());
    _outlines_raw = ofPolylines_ptr(new vector<ofPolyline>());
    _paths = ofPaths_ptr(new vector<ofPtr<ofPath>>());
    _outlines->clear();
    _outlines_raw->clear();
    _paths->clear();

    output_w = w;
    output_h = h;

    RegisterInFactory<ofx2DMappingObject, ofx2DMappingFbo> register1(ofx2DMappingFbo().nature);
    RegisterInFactory<ofx2DMappingObject, ofx2DMappingColorShape> register2(ofx2DMappingColorShape().nature);
    RegisterInFactory<ofx2DMappingObject, ofx2DMappingImage> register3(ofx2DMappingImage().nature);
    RegisterInFactory<ofx2DMappingObject, ofx2DMappingPoint> register4(ofx2DMappingPoint().nature);

}

ofx2DMappingProjector::~ofx2DMappingProjector() {

}

void ofx2DMappingProjector::update() {

    ofPtr<ofx2DMappingObject> mq;
    for(unsigned int i = 0; i < shapeCount(); i++) {
        mq = getMappingObject(i);
        if(mq) {
            if(mq->newpos || mq->newitem) {
                if(!mq->newitem) {
                    updateOutline(i);
                }
                else {
                    updateOutlines();
                }
            }
            mq->update(output_w, output_h);
            mq->newpos = false;
            mq->newitem = false;
        }
    }
}

ofPoint ofx2DMappingProjector::relative(ofPoint orig) {
	return orig*getMatrixOfImageAtPoint(orig);
}

ofPoint ofx2DMappingProjector::inOutput(ofPoint orig) {
    ofPoint res = orig;
    res.x*=outputWidth();
    res.y*=outputHeight();
    return res;
}

ofPoint ofx2DMappingProjector::inCameraView(ofPoint orig) {
    ofPoint res = orig*camera_homography;
    return res;
}

ofPtr<ofx2DMappingObject> ofx2DMappingProjector::addShape(ofPtr<ofx2DMappingObject> obj, bool swap) {
    shapes.push_back(obj);
    updateOutline(shapes.size()-1);
    if(swap) {
        update();
        for(int i = shapes.size()-1; i>0;i--) {
            swapShapes(i, i-1);
        }
        return shapes.at(0);
    }
    return shapes.at(shapes.size()-1);
}

ofPtr<ofx2DMappingObject> ofx2DMappingProjector::addShape(string type, string name, bool swap) {
    ofPtr<ofx2DMappingObject> obj = ofx2DMappingObjectFactory<ofx2DMappingObject>::instance().Create(type);
    if(obj) {
        shapes.push_back(obj);
        updateOutline(shapes.size()-1);
        shapes.at(shapes.size()-1)->name = name;
        if(swap) {
            update();
            for(int i = shapes.size()-1; i>0;i--) {
                swapShapes(i, i-1);
            }
        }
    }
    else {
        ofLogError("Projector: addShape()", "MappingObjectFactory could not create object of type " + type + ".");
    }
    return obj;

}

ofPtr<ofx2DMappingObject> ofx2DMappingProjector::copyShape(ofPtr<ofx2DMappingObject> original, bool swap) {
    int pos = shapes.size();
    shapes.push_back(original->clone());
    updateOutline(pos);
    if(swap) {
        update();
        for(; pos>0;pos--) {
            swapShapes(pos, pos-1);
        }
        pos = 0;
    }
    return shapes.at(pos);
}

bool ofx2DMappingProjector::removeShape(int id) {
    if(id >= 0 && id < (int)shapes.size()) {
        shapes.at(id).reset();
        shapes.erase(shapes.begin()+id);
        return true;
    }
    return false;
}

void ofx2DMappingProjector::removeAllShapes() {
    for(unsigned int i = 0; i < shapes.size(); i++) {
        shapes.at(i).reset();
    }
    shapes.clear();
}

bool ofx2DMappingProjector::swapShapes(int index1, int index2) {
    if(index1 < 0 || index2 < 0) {
        return false;
    }
    if((unsigned int)index1<shapes.size() && (unsigned int)index2 < shapes.size()) {
        std::swap( shapes[index1], shapes[index2] );
        std::swap( _outlines_raw->at(index1), _outlines_raw->at(index2) );
        std::swap( _outlines->at(index1), _outlines->at(index2) );
        return true;
    }
    return false;
}

ofPtr<ofx2DMappingObject> ofx2DMappingProjector::getMappingObject(int id) {
    if(id < (int)shapes.size()) {
        return shapes[id];
    }
    else {
        ofLogError("Projector: getMappingObject()","trying to get object " + ofToString(id) + " but objects size is " + ofToString(shapes.size()));
        return ofPtr<ofx2DMappingObject>();
    }

}

unsigned int ofx2DMappingProjector::shapeCount() {
    return shapes.size();
}

ofPtr<ofx2DMappingObject> ofx2DMappingProjector::getFirstImageShape() {
    ofPtr<ofx2DMappingObject> mq;
    ofPtr<ofx2DMappingObject> mq_res;
    for(unsigned int i = 0; i < shapeCount(); i++) {
        mq = shapes.at(i);
        if(mq) {
            if(std::dynamic_pointer_cast<ofx2DMappingContentShape>(mq)) {
                mq_res = mq;
                break;
            }
        }
    }
    return mq_res;
}

ofMatrix4x4 ofx2DMappingProjector::getMatrixOfImageAtPoint(ofPoint p) {
	vector<ofPtr<ofx2DMappingContentShape>> drawings = getShapesByClass<ofx2DMappingContentShape>();
	for(auto drawing : drawings){
		if(drawing->name == "drawing area"){
			if( pointVisibleInShape(p,drawing)) {
				return drawing->matrix_norm_dst;
			}
		}
    }
    return ofMatrix4x4::newIdentityMatrix();
}

bool ofx2DMappingProjector::pointVisibleInShape(ofPoint p, ofPtr<ofx2DMappingContentShape> mq) {

    ofPoint poly[4];

    unsigned int i, j=3;
    ofPoint last_p = p;
	ofPoint next_p(-10000,10000);
    int intersections = 0;

//    for (i=0; i<4; i++) {
//        poly[i].x = mq->src[i].x * mq->src_width;
//        poly[i].y = mq->src[i].y * mq->src_height;
//    }
	for (i=0; i<4; i++) {
		poly[i].x = mq->dst[i].x * outputWidth();
		poly[i].y = mq->dst[i].y * outputHeight();
	}

    for (i=0; i<4; i++) {

        if(isLeft(poly[i],poly[j], last_p) != isLeft(poly[i],poly[j], next_p)
                && isLeft(next_p,last_p, poly[i]) != isLeft(next_p, last_p, poly[j])) {
            intersections++;
        }
        j=i;
    }

    return intersections%2;
}

bool ofx2DMappingProjector::isLeft(ofPoint a, ofPoint b, ofPoint c){
     return ((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
}

ofPtr<ofxSVG> ofx2DMappingProjector::svg() {
    return _svg;
}

ofPolylines_ptr ofx2DMappingProjector::outlines() {
    return _outlines;
}

void ofx2DMappingProjector::updateOutlines() {

    _outlines->clear();
    _outlines_raw->clear();
    _paths->clear();

    for(unsigned int i = 0; i < shapeCount(); i++) {

        ofPtr<ofx2DMappingObject> mq = getMappingObject(i);

        _paths->push_back(ofPtr<ofPath>(new ofPath()));

        if(ofPtr<ofx2DMappingShape> shape = dynamic_pointer_cast<ofx2DMappingShape>(mq)) {
            _outlines_raw->push_back(shape->polyline);
        }
        else {
            _outlines_raw->push_back(ofPolyline());
        }

        for(unsigned int j = 0; j < _outlines_raw->at(i).size(); j++) {
            _outlines_raw->at(i)[j].x *= output_w;
            _outlines_raw->at(i)[j].y *= output_h;

            if(j == 0)
                ((ofPtr<ofPath>)_paths->at(i))->moveTo(_outlines_raw->at(i)[j]);
            else
                ((ofPtr<ofPath>)_paths->at(i))->lineTo(_outlines_raw->at(i)[j]);
        }

        ((ofPtr<ofPath>)_paths->at(i))->setFillColor(mq->color);

        _outlines->push_back(_outlines_raw->at(i).getResampledBySpacing(1));

    }

}

void ofx2DMappingProjector::updateOutline(int shape_id) {

    if((int)_outlines->size() > shape_id) {
        _outlines->at(shape_id).clear();
        _outlines_raw->at(shape_id).clear();
        _paths->at(shape_id)->clear();
    }
    else {
        _outlines->push_back(ofPolyline());
        _outlines_raw->push_back(ofPolyline());
        _paths->push_back(ofPtr<ofPath>(new ofPath()));
    }

    ofPtr<ofx2DMappingObject> mq = getMappingObject(shape_id);

    if(ofPtr<ofx2DMappingShape> shape = dynamic_pointer_cast<ofx2DMappingShape>(mq)) {
        _outlines_raw->at(shape_id) = shape->polyline.getVertices();
    }

    for(unsigned int j = 0; j < _outlines_raw->at(shape_id).size(); j++) {
        _outlines_raw->at(shape_id)[j].x *= output_w;
        _outlines_raw->at(shape_id)[j].y *= output_h;

        if(j == 0)
            ((ofPtr<ofPath>)_paths->at(shape_id))->moveTo(_outlines_raw->at(shape_id)[j]);
        else
            ((ofPtr<ofPath>)_paths->at(shape_id))->lineTo(_outlines_raw->at(shape_id)[j]);
    }

    ((ofPtr<ofPath>)_paths->at(shape_id))->setFillColor(mq->color);

    _outlines->at(shape_id).addVertices(_outlines_raw->at(shape_id).getResampledBySpacing(1).getVertices());

    _outlines_raw->at(shape_id).close();
    _outlines->at(shape_id).close();
    _paths->at(shape_id)->close();

}

ofPolylines_ptr ofx2DMappingProjector::outlinesRaw() {
    return _outlines_raw;
}

ofPaths_ptr ofx2DMappingProjector::paths() {
    return _paths;
}

void ofx2DMappingProjector::importSvg(string svg) {

    if(reloadSvg(svg)) {
        int outlinescount = _outlines_raw->size();

        for (int j = 0; j < outlinescount; j++) {

            ofColor fill_col = ((ofPtr<ofPath>)_paths->at(j))->getFillColor();
            ofPolyline l = _outlines_raw->at(j);
            ofRectangle bounding = l.getBoundingBox();

            getMappingObject(j)->color = fill_col;

            if(ofPtr<ofx2DMappingShape> shape = dynamic_pointer_cast<ofx2DMappingShape>(getMappingObject(j))) {
                if(l.size() == 4) {
                    shape->dst[0].x = l[0].x/output_w;
                    shape->dst[0].y = l[0].y/output_h;
                    shape->dst[1].x = l[1].x/output_w;
                    shape->dst[1].y = l[1].y/output_h;
                    shape->dst[2].x = l[2].x/output_w;
                    shape->dst[2].y = l[2].y/output_h;
                    shape->dst[3].x = l[3].x/output_w;
                    shape->dst[3].y = l[3].y/output_h;
                }
                else {
                    shape->dst[0].x = bounding.x/output_w;
                    shape->dst[0].y = bounding.y/output_h;
                    shape->dst[1].x = (bounding.x+bounding.width)/output_w;
                    shape->dst[1].y = bounding.y/output_h;
                    shape->dst[2].x = (bounding.x+bounding.width)/output_w;
                    shape->dst[2].y = (bounding.y+bounding.height)/output_h;
                    shape->dst[3].x = bounding.x/output_w;
                    shape->dst[3].y = (bounding.y+bounding.height)/output_h;
                }

                shape->polyline.clear();

                for(unsigned int k = 0; k < l.size(); k++) {
                    shape->polyline.addVertex(l[k].x/output_w, l[k].y/output_h);
                }

                shape->polyline.close();
                shape->newpos = true;
            }

            if(ofPtr<ofx2DMappingPoint> point = dynamic_pointer_cast<ofx2DMappingPoint>(getMappingObject(j))) {
                point->pos.x = l[0].x/output_w;
                point->pos.y = l[0].y/output_h;
                point->newpos = true;
            }

        }
    }

}

bool ofx2DMappingProjector::reloadSvg(string file) {

    ofPtr<ofxSVG> svg = ofPtr<ofxSVG>(new ofxSVG());
    svg->load(file);

    if(svg->getNumPath() == (int)shapeCount()) {
        _svg = svg;
        _outlines_raw->clear();
        _outlines->clear();
        _paths->clear();

        int paths_num = 0;

        for(int j = 0; j<_svg->getNumPath(); j++) {
            _paths->push_back(ofPtr<ofPath>(new ofPath(_svg->getPathAt(j))));
            _outlines_raw->push_back(ofx2DMappingHelper::ofPathToOfPolyline(_svg->getPathAt(j), true));
            _outlines->push_back(_outlines_raw->at(paths_num).getResampledBySpacing(1));
            paths_num++;
        }

        ofLogNotice("Projector: reloadSvg()", "loaded SVG with " + ofToString(paths_num) + " paths");
        return true;
    }
    else {
        ofLogError("Projector: reloadSvg()", "could not import svg because the number of objects in the svg is not the same as in the program. do not remove or add objects to the saved svg. import is only used for position manipulation.");
        return false;
    }

}

void ofx2DMappingProjector::reloadLinesFromRaw() {

    for (int i = 0; i < (int)_outlines_raw->size(); i++){

        _outlines->at(i) = _outlines_raw->at(i).getResampledBySpacing(1);

    }

}

void ofx2DMappingProjector::exportSvg(string path) {
    ofxXmlSettings xml;

    xml.clear();

    xml.addTag("svg");
    xml.addAttribute("svg","id","svg2",0);
    xml.addAttribute("svg","xmlns:rdf","http://www.w3.org/1999/02/22-rdf-syntax-ns#",0);
    xml.addAttribute("svg","xmlns","http://www.w3.org/2000/svg",0);
    xml.addAttribute("svg","width",output_w,0);
    xml.addAttribute("svg","height",output_h,0);
    xml.addAttribute("svg","version","1.1",0);
    xml.addAttribute("svg","xmlns:cc","http://creativecommons.org/ns#",0);
    xml.addAttribute("svg","xmlns:dc","http://purl.org/dc/elements/1.1/",0);
    xml.pushTag("svg", 0);
//        xml.addTag("metadata");
//        xml.addAttribute("metadata","id","metadata7");
//        xml.pushTag("metadata");
//        xml.popTag();
        xml.addTag("g");
        xml.addAttribute("g", "id", "layer1",0);
        xml.pushTag("g");

            int i = 0;
            for(unsigned int j = 0; j < shapeCount(); j++) {

                ofPtr<ofx2DMappingObject> mq = getMappingObject(j);

                if(mq) {

                    xml.addTag("path");
                    stringstream id_sstr;
                    id_sstr << "mappingobject_" << i << "_" << mq->name;
                    xml.addAttribute("path", "id", id_sstr.str(), i);
//                    xml.addAttribute("path", "inkscape:label", mq->nature, i);
                    xml.addAttribute("path", "fill", ofx2DMappingHelper::getColorAsHex(mq->color), i);
                    if(ofPtr<ofx2DMappingShape> shape = dynamic_pointer_cast<ofx2DMappingShape>(mq)) {
                        stringstream path_sstr;
                        path_sstr << "m";
                        ofPoint last_p;
                        for(unsigned int k = 0; k < shape->polyline.size(); k++) {
                            ofPoint cur_p = ofPoint(shape->polyline[k].x*output_w, shape->polyline[k].y*output_h);
                            if(k == 0)
                                path_sstr << cur_p.x << "," << cur_p.y;
                            if(k > 0)
                                path_sstr << "," << cur_p.x-last_p.x << "," << cur_p.y-last_p.y;
                            last_p = cur_p;
                        }
                        path_sstr << "z";

                        xml.addAttribute("path", "d", path_sstr.str(), i);
                        xml.addAttribute("path", "stroke", "#000000", i);
                    }

                    if(ofPtr<ofx2DMappingPoint> shape = dynamic_pointer_cast<ofx2DMappingPoint>(mq)) {
                        stringstream path_sstr;
                        path_sstr << "m";
                        ofPoint cur_p = ofPoint(shape->pos.x*output_w, shape->pos.y*output_h);
                        path_sstr << cur_p.x << "," << cur_p.y << "," << cur_p.x << "," << cur_p.y;
                        path_sstr << "z";

                        xml.addAttribute("path", "d", path_sstr.str(), i);
                        xml.addAttribute("path", "stroke", "#000000", i);
                    }

                    i++;

                }
            }

        xml.popTag();

    xml.popTag();

    xml.saveFile(path);
}

ofParameter<float> &ofx2DMappingProjector::outputWidth() {
    return output_w;
}

ofParameter<float> &ofx2DMappingProjector::outputHeight() {
    return output_h;
}

void ofx2DMappingProjector::setOutputSize(float w, float h){
    output_w = w;
    output_h = h;
}

ofParameter<bool>& ofx2DMappingProjector::getUsingCam() {
    return use_cam;
}

ofPolyline& ofx2DMappingProjector::getCamera() {
   return camera;
}

void ofx2DMappingProjector::setCamera(ofPolyline p){
    camera[0] = p[0];
    camera[1] = p[1];
    camera[2] = p[2];
    camera[3] = p[3];
    //TODO check width and height parameters
    ofx2DMappingObject::findHomography(camera,plane, (GLfloat*)camera_homography.getPtr(),true, outputWidth(), outputHeight());
}
