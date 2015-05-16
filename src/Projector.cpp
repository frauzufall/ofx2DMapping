#include "Projector.h"
#include "Helper.h"
#include "MappingObjectFactory.h"
	
Projector::Projector(float w, float h) {
    shapes.clear();
    shapes.resize(0);
    start_point = ofPoint(0,0);
    plane[0] = ofPoint(0, 0, 0);
    plane[1] = ofPoint(1, 0, 0);
    plane[2] = ofPoint(1, 1, 0);
    plane[3] = ofPoint(0, 1, 0);

    _svg = ofxSVG_ptr(new ofxSVG());
    _outlines = ofPolylines_ptr(new vector<ofPolyline>());
    _outlines_raw = ofPolylines_ptr(new vector<ofPolyline>());
    _paths = ofPaths_ptr(new vector<ofPtr<ofPath>>());
    _outlines->clear();
    _outlines_raw->clear();
    _paths->clear();

    output_w = w;
    output_h = h;


    RegisterInFactory<MappingObject, MappingContentShape> register1(MappingContentShape().nature);
    RegisterInFactory<MappingObject, MappingColorShape> register2(MappingColorShape().nature);
    RegisterInFactory<MappingObject, MappingImage> register3(MappingImage().nature);
}

Projector::~Projector() {

}

void Projector::update() {
    MappingObject_ptr mq;
    for(uint i = 0; i < shapeCount(); i++) {
        mq = getShape(i);
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

ofPoint Projector::relative(ofPoint orig) {
    return orig*getMatrixOfImageAtPoint(orig);
}

MappingObject_ptr Projector::addShape(MappingObject_ptr obj, bool swap) {
    shapes.push_back(obj);
    if(swap) {
        update();
        for(int i = shapes.size()-1; i>0;i--) {
            swapShapes(i, i-1);
        }
        return shapes.at(0);
    }
    return shapes.at(shapes.size()-1);
}

MappingObject_ptr Projector::addShape(string type, bool swap) {
    shapes.push_back(MappingObjectFactory<MappingObject>::instance().Create(type));
    if(swap) {
        update();
        for(int i = shapes.size()-1; i>0;i--) {
            swapShapes(i, i-1);
        }
        return shapes.at(0);
    }
    return shapes.at(shapes.size()-1);
}

bool Projector::removeShape(int id) {
    if(id >= 0 && id < (int)shapes.size()) {
        shapes.at(id).reset();
        shapes.erase(shapes.begin()+id);
        return true;
    }
    return false;
}

void Projector::removeAllShapes() {
    shapes.clear();
}

bool Projector::swapShapes(int index1, int index2) {
    if(index1 < 0 || index2 < 0) {
        return false;
    }
    if((uint)index1<shapes.size() && (uint)index2 < shapes.size()) {
        std::swap( shapes[index1], shapes[index2] );
        std::swap( _outlines_raw->at(index1), _outlines_raw->at(index2) );
        std::swap( _outlines->at(index1), _outlines->at(index2) );
        return true;
    }
    return false;
}

MappingObject_ptr Projector::getShape(int id) {
    if(id < (int)shapes.size()) {
        return shapes[id];
    }
    else {
        cout << "ERROR: projector: trying to get quad " << id << " but quads size is " << shapes.size() << endl;
        return MappingObject_ptr();
    }

}

uint Projector::shapeCount() {
    return shapes.size();
}

MappingObject_ptr Projector::getFirstImageShape() {
    MappingObject_ptr mq;
    MappingObject_ptr mq_res;
    for(uint i = 0; i < shapeCount(); i++) {
        mq = shapes.at(i);
        if(mq) {
            if(std::dynamic_pointer_cast<MappingContentShape>(mq)) {
                mq_res = mq;
                break;
            }
        }
    }
    return mq_res;
}

//template <class T>
//vector<ofPtr<T>> Projector::getShapesByClass() {
//    vector<ofPtr<T>> res;
//    res.clear();
//    ofPtr<T> mo;
//    for(uint i = 0; i < shapeCount(); i++) {
//        mo = dynamic_pointer_cast<T>(getShape(i));
//        if(mo) {
//            res.push_back(mo);
//        }
//    }
//    return res;
//}

ofMatrix4x4 Projector::getMatrixOfImageAtPoint(ofPoint p) {
    vector<MappingContentShape_ptr> images = getShapesByClass<MappingContentShape>();
    for(uint ii = 0; ii < images.size(); ii++) {
        if( pointVisibleInShape(p,images.at(ii))) {
            return images.at(ii)->matrix_src_dst;
        }
    }
    return ofMatrix4x4::newIdentityMatrix();
}

bool Projector::pointVisibleInShape(ofPoint p, MappingContentShape_ptr mq) {

    ofPoint poly[4];

    uint i, j=3;
    ofPoint last_p = p;
    ofPoint next_p(-100,100);
    int intersections = 0;

    for (i=0; i<4; i++) {
        poly[i].x = mq->src[i].x * mq->src_width;
        poly[i].y = mq->src[i].y * mq->src_height;
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

bool Projector::isLeft(ofPoint a, ofPoint b, ofPoint c){
     return ((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
}



ofPoint Projector::getStartPoint() {
    return start_point;
}

void Projector::setStartPoint(ofPoint p) {
    start_point = p;
}

ofxSVG_ptr Projector::svg() {
    return _svg;
}

ofPolylines_ptr Projector::outlines() {
    return _outlines;
}

void Projector::updateOutlines() {

    _outlines->clear();
    _outlines_raw->clear();
    _paths->clear();

    for(uint i = 0; i < shapeCount(); i++) {

        MappingObject_ptr mq = getShape(i);

        _paths->push_back(ofPtr<ofPath>(new ofPath()));

        if(MappingShape_ptr shape = dynamic_pointer_cast<MappingShape>(mq)) {
            _outlines_raw->push_back(shape->polyline);
        }
        else {
            _outlines_raw->push_back(ofPolyline());
        }

        for(uint j = 0; j < _outlines_raw->at(i).size(); j++) {
            _outlines_raw->at(i)[j].x *= output_w;
            _outlines_raw->at(i)[j].y *= output_h;

            //cout << _outlines_raw->at(i)[j] << endl;

            if(j == 0)
                ((ofPtr<ofPath>)_paths->at(i))->moveTo(_outlines_raw->at(i)[j]);
            else
                ((ofPtr<ofPath>)_paths->at(i))->lineTo(_outlines_raw->at(i)[j]);
        }

        ((ofPtr<ofPath>)_paths->at(i))->setFillColor(mq->color);

        _outlines->push_back(_outlines_raw->at(i).getResampledBySpacing(1));

    }

}

void Projector::updateOutline(int shape_id) {

    _outlines->at(shape_id).clear();
    _outlines_raw->at(shape_id).clear();
    _paths->at(shape_id)->clear();

    MappingObject_ptr mq = getShape(shape_id);

    if(MappingShape_ptr shape = dynamic_pointer_cast<MappingShape>(mq)) {
        _outlines_raw->at(shape_id) = shape->polyline.getVertices();
    }

    for(uint j = 0; j < _outlines_raw->at(shape_id).size(); j++) {
        _outlines_raw->at(shape_id)[j].x *= output_w;
        _outlines_raw->at(shape_id)[j].y *= output_h;

        if(j == 0)
            ((ofPtr<ofPath>)_paths->at(shape_id))->moveTo(_outlines_raw->at(shape_id)[j]);
        else
            ((ofPtr<ofPath>)_paths->at(shape_id))->lineTo(_outlines_raw->at(shape_id)[j]);
    }

    ((ofPtr<ofPath>)_paths->at(shape_id))->setFillColor(mq->color);

    _outlines->at(shape_id).addVertices(_outlines_raw->at(shape_id).getResampledBySpacing(1).getVertices());

}

ofPolylines_ptr Projector::outlinesRaw() {
    return _outlines_raw;
}

ofPaths_ptr Projector::paths() {
    return _paths;
}

void Projector::importSvg(string svg) {

    reloadSvg(svg);

    int outlinescount = _outlines_raw->size();

    bool quads_match = ((int)shapeCount()) == outlinescount;

    if(!quads_match) {
        removeAllShapes();
    }

    for (int j = 0; j < outlinescount; j++) {

        ofColor fill_col = ((ofPtr<ofPath>)_paths->at(j))->getFillColor();
        ofPolyline l = _outlines_raw->at(j);
        ofRectangle bounding = l.getBoundingBox();

        if(fill_col.getLightness() == 255) {
            //add content shape
            if(!quads_match) {
                MappingContentShape_ptr cshape = MappingContentShape_ptr(new MappingContentShape());
                addShape(cshape);
            }
        }
        else {
            //add color shape
            if(!quads_match) {
                MappingColorShape_ptr cshape = MappingColorShape_ptr(new MappingColorShape());
                addShape(cshape);
            }
        }

        MappingShape_ptr shape = dynamic_pointer_cast<MappingShape>(getShape(j));

        shape->color = fill_col;

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

        for(uint k = 0; k < l.size(); k++) {
            shape->polyline.addVertex(l[k].x/output_w, l[k].y/output_h);
        }

        shape->polyline.close();
        shape->newpos = true;

    }

}

void Projector::reloadSvg(string file) {

    _svg = ofxSVG_ptr(new ofxSVG());
    _svg->load(file);

    _outlines_raw->clear();
    _outlines->clear();
    _paths->clear();

    int paths_num = 0;

    for(int j = 0; j<_svg->getNumPath(); j++) {
        _paths->push_back(ofPtr<ofPath>(new ofPath(_svg->getPathAt(j))));
        _outlines_raw->push_back(Helper::ofPathToOfPolyline(_svg->getPathAt(j), true));
        _outlines->push_back(_outlines_raw->at(paths_num).getResampledBySpacing(1));
        paths_num++;
    }

    cout << "PROJECTOR:: loaded SVG with " << paths_num << " paths" << endl;

}

void Projector::reloadLinesFromRaw() {

    for (int i = 0; i < (int)_outlines_raw->size(); i++){

        _outlines->at(i) = _outlines_raw->at(i).getResampledBySpacing(1);

    }

}

void Projector::saveMappingAsSvg(string path) {
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
            for(uint j = 0; j < shapeCount(); j++) {

                MappingObject_ptr mq = getShape(j);

                if(mq) {

                    xml.addTag("path");
                    stringstream id_sstr;
                    id_sstr << "path" << i;
                    xml.addAttribute("path", "id", id_sstr.str(), i);
                    xml.addAttribute("path", "fill", Helper::getColorAsHex(mq->color), i);
                    if(MappingShape_ptr shape = dynamic_pointer_cast<MappingShape>(mq)) {
                        stringstream path_sstr;
                        path_sstr << "m";
                        ofPoint last_p;
                        for(uint k = 0; k < shape->polyline.size(); k++) {
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

                    i++;

                }
            }

        xml.popTag();

    xml.popTag();

    xml.saveFile(path);
}
