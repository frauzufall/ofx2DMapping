#pragma once
#include "ofMain.h"
#include "ofx2DMappingController.h"
#include "ofx2DMappingView.h"

class ofx2DMapping {

public:
    ofx2DMapping();
    ~ofx2DMapping();

    void setup(string mapping_path);
    void update();
    void draw(float x, float y, float w, float h);

    ofx2DMappingController* getControl();
    ofx2DMappingView* getView();

private:

    ofx2DMappingController ctrl;
    ofx2DMappingView view;

};
