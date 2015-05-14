#pragma once

#include "MappingContentShape.h"

class MappingImage : public MappingContentShape {

    public:

    ofImage     image;
    string      img_src;

    MappingImage() {
        cout << "MAPPINGIMAGE" << endl;
        this->image.clear();
        this->img_src = "";
        this->nature = "IMAGE";
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        MappingContentShape::loadXml(xml);
        string url = xml->getValue("url", "images/notfound.png");
        image.clear();
        image.load(url);
        img_src = url;
    }

    void draw(float w, float h) {

        cout << "MAPPINGIMAGE::draw()" << endl;

        if(this->image.isAllocated()) {
            ofPushMatrix();
            glMultMatrixf(this->matrix_dst_norm.getPtr());

            ofSetColor(255);
            ofFill();

            this->image.bind();

                glBegin(GL_QUADS);

                glTexCoord2f(this->src[0].x*this->image.getWidth(), this->src[0].y*this->image.getHeight());	glVertex3f(0, 0, 0);
                glTexCoord2f(this->src[1].x*this->image.getWidth(), this->src[1].y*this->image.getHeight());	glVertex3f(w, 0, 0);
                glTexCoord2f(this->src[2].x*this->image.getWidth(), this->src[2].y*this->image.getHeight());	glVertex3f(w, h, 0);
                glTexCoord2f(this->src[3].x*this->image.getWidth(), this->src[3].y*this->image.getHeight());	glVertex3f(0, h, 0);

                glEnd();

            this->image.unbind();
            glPopMatrix();
        }

    }

};
