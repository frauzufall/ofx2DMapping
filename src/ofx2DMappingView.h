#pragma once
#include "ofMain.h"
#include "ofx2DMappingController.h"
#include "ofxSortableList.h"
#include "FormMapping.h"

class ofx2DMappingView {

public:
    ofx2DMappingView();
    void setup(float w, float h);
    void update();
    void draw();
    void draw(ofPoint pos);
    void setMappingBackground(ofFbo_ptr fbo);
    void showSource(bool show);
    void setEditMode(bool &direct_edit);

    void setControl(ofx2DMappingController* ctrl);

    void importSvg();

    FormMapping* getMappingList();

private:

    ofx2DMappingController* ctrl;

    void setSubpanelPositions();
    void updateQuadList();
    void selectAll();
    void deselectAll();

    bool show_source, direct_edit;

    ofRectangle control_rect;

    ofxSortableList shape_list;

    ofPoint pos_main_options, pos_calibration_options, pos_add_buttons, pos_shape_list;

    ofxPanel add_buttons_panel;
    FormMapping mapping_forms;
    ofxPanel main_options, calibration_options, select_options;

    void removeForm(RemovedElementData& data);
    void reorderForm(MovingElementData& data);

    MappingObject_ptr addForm(string type, string name, bool at_bottom);

    ofxButton save_btn, import_btn, select_all_btn, deselect_all_btn;
    ofxToggle edit_mode_btn;
    vector<ofParameter<bool>> add_button_params;

    float zoom;
    ofPoint zoom_pos;


};
