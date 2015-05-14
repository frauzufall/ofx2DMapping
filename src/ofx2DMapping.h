#pragma once
#include "ofMain.h"
#include "ofx2DMappingController.h"
#include "ofx2DMappingView.h"

class ofx2DMapping {

public:
    ofx2DMapping();
    ~ofx2DMapping();

    void setup(string mapping_path, float w=800, float h=600);
    void update();

    ofx2DMappingController* getControl();
    ofx2DMappingView* getView();

private:

    ofx2DMappingController ctrl;
    ofx2DMappingView view;

};
