/**
 * This is a work in progress draft of a simple virtual machine.
 * It's developed with the aim of conciseness and simplicity.
 * The style of this code might not be the best, but it's good enough to work with it.
 * 
 * License: GPL v3
 */

#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <sstream>
#include <vector>
#include <bool>

/** Type of a normal id in the following */
typedef uint64_t id_type;
/** Type of the value of an Int object */;
typedef int64_t int_type; 

/** Types */
enum class Type : id_type {
	NOTHING,
	INT,
	FLOAT,
	OBJECT
};

/**
 * Base class for simple heap objects.
 */
struct HeapObject {
	Type type;
	id_type id;
	uint32_t reference_count = 0;
	id_type class_id;

	/**
	 * Initialize a new HeapObject
	 *
	 * @param id id of the object (or 0 if the id is unknown)
	 * @param type type of the object (@see Type)
	 * @param class_id id of the class this object is an instance of
	 */
	HeapObject(Type type, id_type class_id, id_type id = 0){
		this->id = id;
		this->class_id = class_id;
		this->type = type;
	}

	virtual std::string str(){
		return "";
	}

	virtual std::vector<id_type> getReferencedObjects() {
		return std::vector<id_type>();
	}

	virtual id_type copy(GC *gc){
		return 0;
	}
};

struct Nothing : HeapObject {
	Nothing(id_type id = 0) : HeapObject(Type::NOTHING, (id_type)Type::NOTHING, id) {}
	
	std::string str(){
		return "nothing";
	}
};

/**
 * Int class for integer objects.
 */
struct Int : HeapObject {
	int_type value;

	Int(int_type value, id_type id = 0) : HeapObject(Type::INT, (id_type)Type::INT, id) {
		this->value = value;
	}

	std::string str(){
		std::ostringstream stringStream;
		stringStream << this->value;
		return stringStream.str();
	}
};

typedef std::function<id_type(std::string)> get_function;
typedef std::function<void(std::string, id_type) set_function;

struct Object : HeapObject {
	get_function get_func;
	set_function set_func;

	Object(get_function get_func, set_function set_func, id_type id = 0)
		: HeapObject(Type::OBJECT, 0, id) {
		this->get_func = get_func;
		this->set_func = set_func;
	}
}

enum class LineType : uint8_t {
	
};

/**
 * A line of code
 */
struct Line {
	LineType type;
	
	Line(LineType type){
		this->type = type;
	}
};

// TODO add other line types

struct Function : HeapObject {
	id_type parent_scope_id;
	
	void exec(Env *env);
};

struct GC {
	std::unordered_map<id_type, HeapObject*> heap;
	id_type max_id = 0;
	
	/**
	 * Create an Int object and add it to the heap.
	 */
	Int* createInt(int_type value){
		Int *int_o = new Int(value);
		this->addObject(int_o);
		return int_o; 	
	}

	Nothing* createNothing(){
		Nothing* nothing = new Nothing();
		this->addObject(nothing);
		return nothing;
	}

	/**
	 * Add the passed object to this GC and set its id appropriately.
	 * @param object passed object
	 */
	void addObject(HeapObject *object){
		object->id = this->max_id++;
		this->heap.emplace(object->id, object);
	}

	std::string str(){
		std::ostringstream stream;
		stream << "count=" << heap.size() << "\n";
		for (auto t : this->heap){
			stream << " " << t.first << ":"
			       << "value=" << t.second->str()
			       << ", ref count=" << t.second->reference_count << "\n";
		}
		return stream.str();
	}

	void reference(HeapObject *object){
		object->reference_count++;
	}

	void reference(id_type object_id){
		reference(heap[object_id]);
	}

	/**
	 * Dereference the object and delete it if its reference count is zero
	 */
	void dereference(HeapObject *object){
		if (object->reference_count == 0 || --(object->reference_count) == 0){
			for (auto id : object->getReferencedObjects()){
				dereference(id);
			}	
			heap.erase(object->id);
		}
	}

	/**
	 * Dereference the object and delete it if its reference count is zero
	 */
	void dereference(id_type object_id){
		dereference(heap[object_id]);
	}		

	HeapObject* getAndReference(id_type object_id){
		HeapObject* object = heap[object_id];
		reference(object);
		return object;
	}

	HeapObject* createClassObject(Classes *classes, id_type class_id){
		HeapObject* obj = classes->getClass(class_id)->createInstance();
		addObject(obj);
		return obj;
	}

	id_type copy(id_type object_id){
		return heap[object_id]->copy(this);
	}
};

struct Class {
	id_type id = 0;
	std::string name = "";
	std::vector<Class*> parents;	
	std::map<std::string, id_type> instance_properties;


	HeapObject* createInstance(id_type id = 0){
		
	}

private:
	void addToInstanceMap(GC *gc, std::map<std::string, id_type>& map){
		for (auto& t : instance_properties){
			if (map.find(t.first) == map.end()){
				id_type copied = gc->copy(t.second);
				map[t.first] = copied; 
			}
		}
		for (auto p : parents){
			p->addToInstanceMap(gc, map);
		}
	}
};

std::map<std::string, id_type> instance_properties;
	std::map<std::string, id_type> class_properties;
};

struct Classes {
	std::unordered_map<id_type, Class*> class_id_map;
	std::unordered_map<std::string, Class*> class_name_map;
	size_t max_id = 0;

	Class* getClass(id_type id){
		return class_id_map[id];
	}

	Class* getClass(std::string name){
		return class_name_map[name];
	}

	void addClass(Class *class_obj){
		class_obj->id = max_id++;
		class_id_map[class_obj->id] = class_obj;
		class_name_map[class_obj->name] = class_obj;
	}
};

struct Env {
	GC gc;
	Classes;

	void init(){
		//Add built in classes
	}
}

int main(){
	GC gc;
	Int* integer = gc.createInt(3);
	gc.reference(integer);
	std::cout << gc.str();
	gc.dereference(integer);
	std::cout << gc.str();
}
