#pragma once

#include "MappingObject.h"

class MappingShape : public MappingObject {

public:

    ofPoint     dst[4];
    ofMatrix4x4 matrix_dst_norm;
    ofxTriangle triangle;
    ofMesh      mesh;
    ofPolyline  polyline;
    ofPoint     plane[4];

    MappingShape() {

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

    void loadXml(ofxXmlSettings_ptr xml) {
        MappingObject::loadXml(xml);
        xml->pushTag("dst", 0);
            setRectangle(xml, dst);
        xml->popTag();
        xml->pushTag("polyline");
            polyline = getPolyline(xml);
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

        return res;

    }

};
