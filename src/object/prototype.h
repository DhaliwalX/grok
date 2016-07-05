#ifndef PROTOTYPE_H_
#define PROTOTYPE_H_

#include "object/jsbasicobject.h"

namespace grok {
namespace obj {

/// A transcript from MDN for prototype object
///
/// All objects in JavaScript are descended from Object; all objects inherit 
/// methods and properties from Object.prototype, although they may be overridden 
/// (except an Object with a null prototype, i.e. Object.create(null)). For 
/// example, other constructors' prototypes override the constructor property
/// and provide their own toString() methods. Changes to the Object prototype
/// object are propagated to all objects unless the properties and methods
/// subject to those changes are overridden further along the prototype chain.


/// AddPropertiesFromPrototype ::= copies the properties from the prototype
/// object of the constructor
///     `ctor` ::= constructor object used
///     `obj`  ::= object to which properties will be added
extern void AddPropertiesFromPrototype(JSObject* ctor, JSObject* obj);


}
}

#endif
