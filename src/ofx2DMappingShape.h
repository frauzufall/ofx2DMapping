#pragma once

#include "ofx2DMappingObject.h"

class ofx2DMappingShape : public ofx2DMappingObject {

public:

    ofPoint     dst[4];
    ofMatrix4x4 matrix_dst_norm;
    ofxTriangle triangle;
    ofPolyline  polyline;
    ofPoint     plane[4];

    ofx2DMappingShape() {

        this->nature = "SHAPE";

        this->dst[0].x = 0;
        this->dst[0].y = 0;
        this->dst[1].x = 1;
        this->dst[1].y = this->dst[0].y;
        this->dst[2].x = this->dst[1].x;
        this->dst[2].y = 1;
        this->dst[3].x = this->dst[0].x;
        this->dst[3].y = this->dst[2].y;

        this->polyline.clear();
        this->polyline.addVertex(this->dst[0]);
        this->polyline.addVertex(this->dst[1]);
        this->polyline.addVertex(this->dst[2]);
        this->polyline.addVertex(this->dst[3]);
        this->polyline.close();

        this->plane[0] = ofPoint(0, 0, 0);
        this->plane[1] = ofPoint(1, 0, 0);
        this->plane[2] = ofPoint(1, 1, 0);
        this->plane[3] = ofPoint(0, 1, 0);

        this->matrix_dst_norm.makeIdentityMatrix();
    }

    ofx2DMappingShape(const ofx2DMappingShape& obj) : ofx2DMappingObject(obj) {
        for(int i = 0; i < 4; i++) {
            this->dst[i] = obj.dst[i];
            this->plane[i] = obj.plane[i];
        }
        this->polyline = obj.polyline;
        this->triangle = obj.triangle;
        this->matrix_dst_norm = obj.matrix_dst_norm;
    }

    ofPtr<ofx2DMappingObject> clone() const {
        return ofPtr<ofx2DMappingObject>(new ofx2DMappingShape(*this));
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        ofx2DMappingObject::loadXml(xml);
        xml->pushTag("dst", 0);
            setRectangle(xml, dst);
        xml->popTag();
        xml->pushTag("polyline");
            polyline = getPolyline(xml);
        xml->popTag();
    }

    void saveXml(ofxXmlSettings_ptr xml) {
        ofx2DMappingObject::saveXml(xml);
        xml->addTag("dst");
        xml->pushTag("dst", 0);
            xml->addTag("lefttop");
            xml->pushTag("lefttop", 0);
                xml->addValue("x", dst[0].x);
                xml->addValue("y", dst[0].y);
            xml->popTag();
            xml->addTag("righttop");
            xml->pushTag("righttop", 0);
                xml->addValue("x", dst[1].x);
                xml->addValue("y", dst[1].y);
            xml->popTag();
            xml->addTag("rightbottom");
            xml->pushTag("rightbottom", 0);
                xml->addValue("x", dst[2].x);
                xml->addValue("y", dst[2].y);
            xml->popTag();
            xml->addTag("leftbottom");
            xml->pushTag("leftbottom", 0);
                xml->addValue("x", dst[3].x);
                xml->addValue("y", dst[3].y);
            xml->popTag();
        xml->popTag();

        xml->addTag("polyline");
        xml->pushTag("polyline",0);

            for(uint k = 0; k < polyline.size(); k++) {
                xml->addTag("point");
                xml->pushTag("point",k);
                    xml->addValue("x", polyline[k].x);
                    xml->addValue("y", polyline[k].y);
                xml->popTag();
            }

        xml->popTag();
    }

    void draw(float w, float h){}

    void drawArea(float w, float h) {

        ofSetColor(this->color);

        ofFill();

        ofBeginShape();
        for (uint j = 0; j < this->polyline.size(); j++){
            ofVertex(this->polyline[j].x*w, this->polyline[j].y*h);
        }

        ofEndShape(true);

    }

    void update(float w, float h) {
        this->findHomography(this->plane, this->dst, (GLfloat*) this->matrix_dst_norm.getPtr(), true, w, h);
        this->triangle.clear();
        this->triangle.triangulate(this->polyline.getVertices());
    }

protected:

    void setRectangle(ofxXmlSettings_ptr xml, ofPoint *rectangle) {

        xml->pushTag("lefttop", 0);
            rectangle[0] = getPoint(xml);
        xml->popTag();
        xml->pushTag("righttop", 0);
            rectangle[1] = getPoint(xml);
        xml->popTag();
        xml->pushTag("rightbottom", 0);
            rectangle[2] = getPoint(xml);
        xml->popTag();
        xml->pushTag("leftbottom", 0);
            rectangle[3] = getPoint(xml);
        xml->popTag();

    }

    ofPolyline getPolyline(ofxXmlSettings_ptr xml) {

        ofPolyline res;

        int point_count = xml->getNumTags("point");

        for(int k = 0; k < point_count; k++) {
            xml->pushTag("point",k);
                ofPoint point(xml->getValue("x", 0.), xml->getValue("y", 0.));
                res.addVertex(point);
            xml->popTag();
        }

        res.close();

        return res;

    }

};
