#pragma once

#include "ofx2DMappingContentShape.h"

class ofx2DMappingFbo : public ofx2DMappingContentShape {

    public:

    ofFbo_ptr fbo;

    ofx2DMappingFbo() {
        this->nature = "FBO";
    }

    ofx2DMappingFbo(const ofx2DMappingFbo& obj) : ofx2DMappingContentShape(obj) {
        this->fbo = obj.fbo;
    }

    ofPtr<ofx2DMappingObject> clone() const {
        return ofPtr<ofx2DMappingObject>(new ofx2DMappingFbo(*this));
    }

    void setFbo(ofFbo_ptr &fbo) {
        this->fbo = fbo;
        if(fbo) {
            src_width = fbo->getWidth();
            src_height = fbo->getHeight();
            setTexture(&fbo->getTexture());
        }
    }

};
