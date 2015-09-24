#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

template<typename T>
class _OptionalHelper {
public:
    T t;
    _OptionalHelper(T t) : t(t) {}
};

/**
 * @brief An optional type comparable to std::optional or Haskells Maybe.
 */
template<typename T>
class Optional {

    _OptionalHelper<T>* t;
    bool _has = true;

public:
    Optional(T t) : t(new _OptionalHelper<T>(t)){}

    Optional() : _has(false) {}

    bool has(){
        return _has;
    }

    T value(){
        if (!_has){
            throw std::string("No value present in Optional of ") + std::string(typeid(T).name());
        }
        return t->t;
    }

    ~Optional(){
        //delete t;
    }

    Optional<T> operator=(Optional<T> r){
        if (r.has()){
            _has = true;
            t = new _OptionalHelper<T>(r.value());
        } else {
            _has = false;
        }
        return r;
    }
};

#endif
