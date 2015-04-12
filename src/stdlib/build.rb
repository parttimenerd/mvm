#! /usr/bin/ruby
require 'digest'

$dir = File.expand_path File.dirname(__FILE__)

require 'pp'

def process_type type, var
	case type
	when "numeric"
		"#{var}->value->isNumeric()"
	when "map"
		"#{var}->value->isMap()"
	when "function"
		"#{var}->value->isFunction()"
	else
		"#{var}->value->type == Type::#{process_raw_type type}"
	end
end

def process_type_str str, var
	arr = str.split("/")
	arr.map! do |a|
		process_type a, var
	end
	arr.join(" || ")
end

def process_raw_type type
	{"int" => "INT",
 	 "float" => "FLOAT",
	 "array" => "ARRAY",
	 "map" => "MAP", 
	 "nothing" => "NOTHING", 
	 "boolean" => "BOOLEAN",
	 "string" => "STRING",
	 "scope" => "SCOPE",
	 "function" => "FUNCTION"}[type]
end

def parse_type_str str, has_variable_params, context
	arr = str.split("->")
	processed = []
	(0.upto(arr.length - (has_variable_params ? 2 : 1))).each do |i|
		var = "args.at(#{i})"
		processed << create_type_exception_str("(#{process_type_str arr[i], var})", i,context, arr[i])
	end
	ret = processed.join("")
	if has_variable_params
		last = arr[arr.length - 1]
		var = "args.at(i)"
		ret += "
		for (size_t i = #{arr.length - 2}; i < args.all_arguments.size(); i++){
                        #{create_type_exception_str "(#{process_type last, "args.at(i)"})", "i", context, last}
		}
		"
	end
	ret
end

def create_type_exception_str condition, param_num, context, expected
	"
		if (!#{condition}){
			throw new Exception(\"wrong argument type\", \"Expected #{expected} but got \" + args.at(#{param_num})->value->str(), #{context});
		}
"
end	

def add_func_str param, is_statement, file, line_num	
	arr = param.split(",")
	func_name = arr[0]
	cpp_name = func_name
	if func_name.split("|").length == 2
		a = func_name.split("|")
		func_name = a[0]
		cpp_name = a[1]
	end	
	context = create_context func_name, file, line_num
	param_count = arr.length == 2 ? arr[1] : "0"
	arrr = param_count.split("|")
	enforce = true
	if arrr.length > 1
		enforce = arrr[1] != "var"
		param_count = arrr[0]
	end
	enforce_str = ""
	if enforce
		enforce_str = "
		if (args.hasTooMuchArguments()){
			std::ostringstream stream;
			stream << \"Expected #{param_count} arguments, but got \";
			stream << args.all_arguments.size();
			throw new Exception(\"too much arguments\", stream.str(), #{context});	
		}
"
	end
	if (arrr[1] == "var" && arrr.length > 2) || (arrr[1] != "var" && arrr.length > 1)
		enforce_str += parse_type_str arrr[-1], !enforce, context
	end
	"
	env->addFunction(#{context}, \"#{func_name}\", #{param_count}, [](Env *env, FunctionArguments &args){
		#{enforce_str}
		#{is_statement ?  "" : "return "}#{cpp_name}(env, args);
		#{is_statement ? "return env->snothing().reference;" : ""} 
	});"
end

def create_context func_name, file, line_num
	"ExceptionContext(LangContext(new std::string(\"#{file}\"), #{line_num}, 0), \"#{func_name}\")"
end

def process file
	func_str = ""
	lines = File.readlines($dir + "/" + file, "\n")
	num = 0
	lines.each do |line|
		num += 1
		if line.start_with?("//statement ") || line.start_with?("//function ")
			func_str += add_func_str line.gsub("//statement ", "").gsub("//function ", "").gsub(/[\);\n ]/, ""), line.start_with?("//statement"), file, num
		end
	end
	func_str
end

md5 = ""
Dir.foreach($dir) do |file|
	if file.end_with?(".hpp") && file != "stdlib.hpp" && file != "utils.hpp"
		md5 += Digest::MD5.file($dir + "/" + file).hexdigest 
	end
end

if File.exists?($dir + "/digest") && md5 == File.read($dir + "/digest")
	exit(0)
else
	File.open($dir + "/digest", "w") do |f|
		f.print md5
	end
end


str = ""
includes = ""
Dir.foreach($dir) do |file|
	if file.end_with?(".hpp") && file != "stdlib.hpp" && file != "utils.hpp"
		str += process(file)
		includes += "#include \"#{file}\"\n"
	end
end

File.open($dir + "/stdlib.hpp", "w") do |f|
	f.puts <<EOF
#pragma once 
/** Don't modify this file. It's auto generated. */

#include "utils.hpp"
#{includes}

void loadStdLib(Env *env){
	#{str}
}

EOF
end
