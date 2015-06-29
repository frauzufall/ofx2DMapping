#pragma once

template <class ManufacturedType, typename ClassIDKey=std::string>
class ofx2DMappingObjectFactory
{
    typedef ofPtr<ManufacturedType> (*BaseCreateFn)();
    typedef std::map<ClassIDKey, BaseCreateFn> FnRegistry;
    FnRegistry registry;

    ofx2DMappingObjectFactory(){}

public:
    static ofx2DMappingObjectFactory &instance() {
        static ofx2DMappingObjectFactory bf;
        return bf;
    }

    void RegCreateFn(const ClassIDKey &className, BaseCreateFn fn){
        registry[className] = fn;
    }

    ofPtr<ManufacturedType> Create(const ClassIDKey &className) const {
        ofPtr<ManufacturedType> theObject(0);
        typename FnRegistry::const_iterator regEntry = registry.find(className);
        if (regEntry != registry.end())
        {
            theObject = regEntry->second();
        }
        return theObject;
    }

};

template <class AncestorType, class ManufacturedType, typename ClassIDKey=std::string>
class RegisterInFactory {

public:
    static ofPtr<AncestorType> CreateInstance() {
        return ofPtr<AncestorType>(new ManufacturedType);
    }

    RegisterInFactory(const ClassIDKey &id) {
        ofx2DMappingObjectFactory<AncestorType, ClassIDKey>::instance().RegCreateFn(id, CreateInstance);
    }
};

