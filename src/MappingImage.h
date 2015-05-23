#pragma once

#include "MappingContentShape.h"

class MappingImage : public MappingContentShape {

    public:

    ofImage     image;
    string      img_src;

    MappingImage() {
        this->image.clear();
        this->img_src = "";
        this->nature = "IMAGE";
    }

    void copy(ofPtr<MappingImage> obj)  {
        MappingContentShape::copy(obj);
        this->image = obj->image;
        this->img_src = obj->img_src;
    }

    void loadXml(ofxXmlSettings_ptr xml) {
        MappingContentShape::loadXml(xml);
        string url = xml->getValue("url", "images/notfound.png");
        image.clear();
        image.load(url);
        img_src = url;
    }

    void draw(float w, float h) {

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

    ofTexture* getTexture() {
        if(image.isAllocated())
            return &image.getTexture();
        else
            return 0;
    }

};
