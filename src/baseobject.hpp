#ifndef BASEOBJECT_HPP
#define BASEOBJECT_HPP

#include "utils.hpp"

#include "smartreference.hpp"
#include "function_arguments.hpp"
#include "heapobject.hpp"

class Class;
class Env;
class Reference;

class BaseObject : public HeapObject {

protected:
    Optional<SmartReference<Class>> class_obj;

    /**
     * Contains all non function members and caches bound functions.
     */
    std::map<std::string, rref> members;

public:
    BaseObject(Env *env, SmartReference<Class> obj_class, Type type = Type::OBJECT);

    BaseObject(Env *env, std::string class_name);

    BaseObject(Env *env);

    /**
     * @brief Get the class that this object belongs to.
     * @return the object's class
     */
    virtual SmartReference<Class> getClass();

    /**
     * @brief Get a reference to the member.
     *
     * @param member_name name of the member
     * @return member or nothing
     */
    virtual rref get(std::string member_name);

    /**
     * @brief Set the members value
     *
     * @param member_name name of the member
     * @param value new value
     * @return the new value
     */
    virtual rref set(std::string member_name, rref value);

    /**
     * @brief Set the members value
     *
     * @param member_name name of the member
     * @param value new value
     * @return the new value
     */
    virtual rref set(std::string member_name, bref value);

    /**
     * @brief Does this object has the member X?
     * @param member_name X
     * @return Does this object has the member X?
     */
    virtual bool has(std::string member_name);

    /**
     * @brief Does this object has the passed type?
     * @param type passed type
     */
    virtual bool is(std::string type);

    /**
     * @brief Call the method
     *
     * @param method name of the method
     * @param args arguments
     */
    template<class... Args>
    rref call(std::string method, Args... args){
        std::vector<rref> vec = {args...};
        return call(method, vec);
    }

    /**
     * @brief Call the method
     *
     * @param method name of the method
     * @param args arguments
     */
    virtual rref call(std::string method, std::vector<rref> args);

    /**
     * @brief Call the method
     * Copies the references of the passed FunctionArguments object.
     *
     * @param method name of the method
     * @param args arguments
     */
    virtual rref call(std::string method, FunctionArguments& args);

    /**
     * @brief Call the method if it's really an existing method
     *
     * @param method name of the method
     * @param args arguments
     */
    template<class... Args>
    rref maybeCall(std::string method, Args... args){
        if (has(method)){
            return call(method, args...);
        }
        return __createNothing();
    }

    /**
     * @brief Call the method if it's really an existing method
     * Copies the references of the passed FunctionArguments object.
     *
     * @param method name of the method
     * @param args arguments
     */
    virtual rref maybeCall(std::string method, FunctionArguments& args){
        if (has(method)){
            return call(method, args);
        }
        return __createNothing();
    }

    virtual bref bind(rref this_obj, std::string name);

    virtual void setDirect(bref other);

    /**
     * @brief Clone this object and all it's members.
     * @return clone of this object
     */
    virtual bref clone();

    virtual bool equals(bref other);

    virtual bool equals(rref other);

    virtual bool hasSameClass(bref other);

    virtual rref call(std::vector<rref> &args);

protected:

    void copyMembers(bref other);

    virtual bool _equals(bref) { return false; }

    virtual void _setDirect(bref) { }

    rref __createNothing();
};

#endif // BASEOBJECT_HPP
