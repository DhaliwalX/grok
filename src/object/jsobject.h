#ifndef JS_OBJECT_H_
#define JS_OBJECT_H_

#include "object/object.h"
#include "common/exceptions.h"
#include "object/jsbasicobject.h"
#include "object/jsnumber.h"
#include "object/jsstring.h"

#include <memory>
#include <string>

namespace grok { namespace obj {
extern std::string __type[7];

#define DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(op, type) \
    { throw std::runtime_error(  \
            std::string("operator '" #op "' is not defined for '" \
                #type "'")); }

#define DEFINE_OPERATOR(op, type)       \
    virtual Object operator op(Object ) \
        DEFAULT_RETURN_FOR_UNDEFINED_OPERATOR(op, type)

#define DECLARE_OTHER_OPERATOR(op) \
extern Object operator op (Object l, Object r)

DECLARE_OTHER_OPERATOR(+);
DECLARE_OTHER_OPERATOR(*);
DECLARE_OTHER_OPERATOR(-);
DECLARE_OTHER_OPERATOR(/);
DECLARE_OTHER_OPERATOR(%);
DECLARE_OTHER_OPERATOR(==);
DECLARE_OTHER_OPERATOR(!=);
DECLARE_OTHER_OPERATOR(<);
DECLARE_OTHER_OPERATOR(>);
DECLARE_OTHER_OPERATOR(<=);
DECLARE_OTHER_OPERATOR(>=);
DECLARE_OTHER_OPERATOR(>>);
DECLARE_OTHER_OPERATOR(<<);
DECLARE_OTHER_OPERATOR(&);
DECLARE_OTHER_OPERATOR(|);
DECLARE_OTHER_OPERATOR(||);
DECLARE_OTHER_OPERATOR(&&);
DECLARE_OTHER_OPERATOR(^);
#undef DECLARE_OTHER_OPERATOR

}
}
#endif

