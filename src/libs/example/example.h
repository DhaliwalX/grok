#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include <functional>   // std::bind
#include <iostream> // std::cout, std::endl

#include "object/jsbasicobject.h"   // grok::obj::JSObject
#include "object/function.h"    // grok::obj::Function

namespace example {

// Every object definition like JSNumber, JSDouble, JSObject, JSArray,
// Argument, FunctionTemplate, Function, UndefinedObject JSNull lies in
// this namespace
//
// One class Handle is different, it used as a handle to store the objects
// it is functionally similar to boost::any
using namespace grok::obj;

/// Example ::= An Example class to show C++ binding with JavaScript
/// ====---------------------------------------------------------=====
///
/// This class has one data member a_ and one member function SayHello
/// which act as a javascript method sayHello. A new object of Example
/// can be created in JavaScript using `var a = new Example(10);`.  You
/// can call SayHello from code `a.sayHello();` and it will print 'Hello 10'
/// to console.
/// 
/// Every class, which you want to bind, must inherit from JSObject, a base
/// from which nearly every other class in grok::obj namespace inherits.
/// You will inherit AddProperty, GetProperty, HasProperty, RemoveProperty
/// ToString, AsString, GetType etc member functions which you can override
/// if you want different behavior, just remember to mark it override ;-).
class Example : public JSObject {
public:
    Example(double a) :
        a_ { a }
    {
        Init();
    }

    /// SayHello ::= member function which we want to bind with javascript
    /// objects
    /// All functions must have atleast following prototype. Return type
    /// should be always be std::shared_ptr to Object class.
    ///
    /// If you want to return nothing to javascript code, then just
    /// insert a statement `return CreateUndefinedObject();` so as to avoid
    /// undefined behavior.
    std::shared_ptr<Handle> SayHello(std::shared_ptr<Argument>)
    {
        std::cout << "Hello " << a_ << std::endl;

        // return something but never nullptr, It should be a valid pointer
        // to a valid object.
        // CreateUndefinedObject returns a handle to UndefinedObject shared_ptr
        return CreateUndefinedObject(); 
    }

    /// Init ::= Initialise the properties of this example object
    void Init()
    {
        // We can use member function as a JavaScript method just because of
        // std::bind. We are creating a callable object `func` which will be
        // used as a javascript property.
        // 
        // here we are using `bind` to create a function object whose default
        // parameter is `this` pointer(required), and we leave place for
        // std::shared_ptr<Argument> by using `std::placeholders::_1`. Now
        // bind will create a function whose prototype is same as following
        // ```
        //      std::shared_ptr<Object> func(std::shared_ptr<Argument>)
        // ```
        // If you want more default arguments which should be passed for every
        // call, you will just add them here in std::bind. If you want learn
        // more about std::bind, search google.
        auto func = std::bind(&Example::SayHello, this, std::placeholders::_1);
        
        // create a JavaScript function from the above func object
        auto prop = CreateFunction(func);

        // add this method to the object as a javascript property
        this->AddProperty("sayHello", prop);
    }

    /// ExampleObjectConstructor ::= constructor that will be called from
    /// JS code to create `Example` objects. The created object will be
    /// returned as a handle to the actual object. Note the prototype, it is
    /// same as SayHello.
    static std::shared_ptr<Handle>
    ExampleObjectConstructor(std::shared_ptr<Argument> args)
    {
        // args has all the arguments (as its properties) that are passed
        // to this function from javascript code even extra args.
        //
        // In this function, we find the `num` object in args object.
        auto arg = args->GetProperty("num");
        double num = 0; 

        // next we check whether num is undefined
        if (!IsUndefined(arg)) {

            // if it is not undefined then we explicitly will convert the
            // `num` to string so that later we can try to convert to number.
            // This is unnecessary but safe. 
            auto str = arg->as<JSObject>()->ToString();

            // we try to create number from string, if successful CreateJSNumber
            // will return a fresh made number object from which we can extract
            // actual number, if failed CreateJSNumber will return undefined
            // object
            auto n = CreateJSNumber(str);
            if (IsUndefined(n))
                num = 0;
            else {
                num = n->as<JSDouble>()->GetNumber();
            }
        }

        // create shared_ptr to the Example object
        auto ex = std::make_shared<Example>(num);

        // this function adds the default properties to the Example object
        // like toString etc.
        DefineInternalObjectProperties(ex.get());

        // create a handle that will point to the shared_ptr of actual object.
        auto ex_wrapped = std::make_shared<Handle>(ex);

        return ex_wrapped;
    }

    /// CreateConstructor ::= a helper function that will be called
    /// to create example constructor and add to the VM's the VStore
    /// (a place where all object at runtime lies)
    static std::shared_ptr<Handle>
    CreateConstructor()
    {
        auto func_w = CreateFunction(ExampleObjectConstructor);
        auto func = func_w->as<Function>();
        func->SetParams({ "num" });
        return func_w;
    }
private:
    double a_;
};

}

#endif
