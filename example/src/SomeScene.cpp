#include "SomeScene.h"

SomeScene::SomeScene()
{
}

void SomeScene::setup(float w, float h) {
    fbo = ofPtr<ofFbo>(new ofFbo());
    fbo->allocate(w,h,GL_RGBA);
    fbo->begin();
    ofClear(0,0,0,0);
    fbo->end();
}

void SomeScene::update() {

    int time_i = ofGetElapsedTimeMillis();

    fbo->begin();

    ofSetColor(ofColor::fromHsb((int)(time_i*0.05)%255, 255, 255));
    ofSetLineWidth(10);

    ofDrawLine(
                0,
                fbo->getHeight()/32*(time_i%32),
                fbo->getWidth(),
                fbo->getHeight()/32*(time_i%32)
    );

    fbo->end();

}

ofPtr<ofFbo> SomeScene::getFbo() {
    return fbo;
}
