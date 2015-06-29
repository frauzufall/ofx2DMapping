# ofx2DMapping
2D mapping tool for different object types like ofFbo, ofImage or completely custom objects.

##Features
- adding mapping objects containing 
  - ofFbo 
  - ofImage 
  - ofColor 
  - or custom contents implemented by a derived class
- manipulate mapping objects
  - change geometry
  - change order
  - delete object
  - crop content
- direct or indirect editing
- indirect edit mode with zoom function
- calibration mode
- SVG export & import (only meant to be used for geometry changes like moving points or adding vertices, not for object removal or addition; tested with Inkscape)

##Usage
Tested with linux 64bit. See example for usage details.

##Installation
Please use the lastest git version of openFrameworks until v0.9 is published. Otherwise the addon will crash. 
Additionally to this addon, you need **ofxSortableList** (https://github.com/frauzufall/ofxSortableList).
