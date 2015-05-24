#include "ofx2DMappingView.h"

ofx2DMappingView::ofx2DMappingView() {

    control_rect.position = ofPoint(0,0);
    zoom = 1;
    zoom_pos = ofPoint(0.5,0.5);

}

void ofx2DMappingView::setControl(ofx2DMappingController *ctrl) {
    this->ctrl = ctrl;
}

void ofx2DMappingView::setup(float w, float h) {

    control_rect.width= w;
    control_rect.height= h;

    direct_edit = false;

    //MAPPING RECT PANEL
    mapping_forms.setup("MAPPING FORMS", ctrl->getProjector(0), &shape_list, w, h);
    mapping_forms.rebuild();
    mapping_forms.setMappingBackground(ctrl->getOutput());
    ofAddListener(ctrl->getProjector(0)->updatedFbo, &mapping_forms, &FormMapping::setMappingBackground);

    //MAIN OPTIONS PANEL

    main_options.setup("MAPPING");

    save_btn.addListener(ctrl, &ofx2DMappingController::saveMappingDefault);
    save_btn.setup("save");
    main_options.add(&save_btn);

    import_btn.addListener(this, &ofx2DMappingView::importSvg);
    import_btn.setup("import svg");
    main_options.add(&import_btn);

    edit_mode_btn.addListener(this, &ofx2DMappingView::setEditMode);
    edit_mode_btn.setup("direct edit", direct_edit);
    main_options.add(&edit_mode_btn);

    //ADDING BUTTONS PANEL

    add_buttons_panel.setup("ADD MAPPING OBJECTS");

    add_button_params.clear();
    vector<MappingObject_ptr> options = ctrl->getOptions();
    for(uint i = 0; i < options.size(); i++) {
        add_button_params.push_back(ofParameter<bool>("add " + options.at(i)->name, false));
        add_buttons_panel.add(add_button_params.at(i));
    }

    //CALIBRATION OPTIONS

    calibration_options.setup("CALIBRATION OPTIONS");

    calibration_options.add(ctrl->getCalibrating());
    calibration_options.add(ctrl->getCalBorder());
    calibration_options.add(ctrl->getCalGrey());

    //QUAD LIST

    shape_list.setup("MAPPING FORM LIST");
    ofAddListener(shape_list.elementRemoved, this, &ofx2DMappingView::removeForm);
    ofAddListener(shape_list.elementMovedStepByStep, this, &ofx2DMappingView::reorderForm);
    shape_list.setHeaderBackgroundColor(ofColor::black);

    //SELECT OPTIONS

    select_options.setup("FORM SELECTION");
    select_all_btn.addListener(this, &ofx2DMappingView::selectAll);
    select_all_btn.setup("select all");
    select_options.add(&select_all_btn);

    deselect_all_btn.addListener(this, &ofx2DMappingView::deselectAll);
    deselect_all_btn.setup("deselect all");
    select_options.add(&deselect_all_btn);

    setSubpanelPositions();

    updateQuadList();


}

void ofx2DMappingView::update() {

    mapping_forms.update();

    for(uint i = 0; i < add_button_params.size(); i++) {
        if(add_button_params.at(i)) {
            MappingObject_ptr obj = ctrl->getOptions().at(i);
            MappingObject_ptr copy = ctrl->getProjector(0)->copyShape(obj);
            ctrl->getProjector(0)->addListeners(copy);
            mapping_forms.updateForms();
            updateQuadList();
            add_button_params.at(i) = false;
        }
    }

}

void ofx2DMappingView::draw() {
    draw(ofPoint(0,0));
}

void ofx2DMappingView::draw(ofPoint pos) {

    if(pos != control_rect.position) {
        control_rect.position = pos;
        mapping_forms.setPosition(control_rect.x, mapping_forms.getPosition().y);
        mapping_forms.rebuild();
        setSubpanelPositions();
    }

    ofPushStyle();

    mapping_forms.draw(show_source);

    shape_list.draw();
    select_options.draw();
    main_options.draw();
    calibration_options.draw();
    add_buttons_panel.draw();

    ofPopStyle();

}

void ofx2DMappingView::updateQuadList() {

    shape_list.clear();

    Projector *p = ctrl->getProjector(0);

    for(uint i = 0; i < p->shapeCount(); i++) {

        MappingObject_ptr mq = p->getShape(i);
        if(mq) {
            string objname = mq->name;
            //insert toggles at beginning of list
            mq->editable.set(objname, true);
            shape_list.add(mq->editable,false);
            shape_list.getListItems().at(0)->setBorderColor(mq->color);
        }
    }
}

void ofx2DMappingView::importSvg() {
    ctrl->importSvg();
    mapping_forms.updateForms();
    shape_list.clear();
    updateQuadList();
}

MappingObject_ptr ofx2DMappingView::addForm(string type, string name, bool at_bottom) {
    MappingObject_ptr mq = ctrl->getProjector(0)->addShape(type, at_bottom);
    mapping_forms.updateForms();
    mq->editable.set(name,true);
    shape_list.add(mq->editable,at_bottom);
    if(at_bottom) {
        shape_list.getListItems().at(shape_list.getListItems().size()-1)->setBorderColor(mq->color);
    }
    else {
        shape_list.getListItems().at(0)->setBorderColor(mq->color);
    }
    return mq;
}

void ofx2DMappingView::removeForm(RemovedElementData &data) {

    Projector *p = ctrl->getProjector(0);
    int index = p->shapeCount()-1-data.index;
    ctrl->getProjector(0)->removeListeners(p->getShape(index));
    if(p->removeShape(index)) {
//        if(p->getFirstImageShape() == 0) {
//            addForm("CONTENT_SHAPE", "painting", true);
//        }
        p->updateOutlines();
        mapping_forms.updateForms();
    }

}

void ofx2DMappingView::reorderForm(MovingElementData &data) {

    Projector *p = ctrl->getProjector(0);

    int index1 = p->shapeCount()-1-data.old_index;
    int index2 = p->shapeCount()-1-data.new_index;

    bool swapped = p->swapShapes(index1,index2);
    if(swapped) {
        mapping_forms.updateForms();
    }

}

void ofx2DMappingView::setSubpanelPositions() {
    float margin = 10;
    main_options.setPosition(
                control_rect.x+margin,
                control_rect.y+margin);
    calibration_options.setPosition(
                main_options.getPosition().x + main_options.getWidth() + margin,
                main_options.getPosition().y);
    mapping_forms.setPosition(
                main_options.getPosition().x,
                main_options.getPosition().y+main_options.getHeight()+margin);
    add_buttons_panel.setPosition(
                mapping_forms.getPosition().x + mapping_forms.getWidth()+margin,
                mapping_forms.getPosition().y);
    shape_list.setPosition(
                add_buttons_panel.getPosition().x,
                add_buttons_panel.getPosition().y + add_buttons_panel.getHeight()+margin);
    select_options.setPosition(
                calibration_options.getPosition().x + calibration_options.getWidth() + margin,
                calibration_options.getPosition().y);
}

void ofx2DMappingView::setMappingBackground(ofFbo_ptr fbo) {
    mapping_forms.setMappingBackground(fbo);
}

void ofx2DMappingView::showSource(bool show) {
    show_source = show;
}

void ofx2DMappingView::setEditMode(bool &direct_edit) {
    this->direct_edit = direct_edit;
    mapping_forms.setEditMode(direct_edit);
    setSubpanelPositions();
}

FormMapping* ofx2DMappingView::getMappingList() {
    return &mapping_forms;
}

void ofx2DMappingView::selectAll() {
    for(uint i = 0; i < ctrl->getProjector(0)->shapeCount(); i++) {
        ctrl->getProjector(0)->getShape(i)->editable = true;
    }
}

void ofx2DMappingView::deselectAll() {
    for(uint i = 0; i < ctrl->getProjector(0)->shapeCount(); i++) {
        ctrl->getProjector(0)->getShape(i)->editable = false;
    }
}
