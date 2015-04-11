#! /usr/bin/ruby
$dir = File.expand_path File.dirname(__FILE__)

require 'pp'

def add_statement_str func_name, file, line_num
	context = create_context func_name, file, line_num
	"
	env->addFunction(#{context}, \"#{func_name}\", 0, [](Env *env, FunctionArguments &args){
		#{func_name}(env, args);
		return env->snothing().reference; 
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
		if line.start_with? "//STATEMENT("
			func_str += add_statement_str line.gsub("//STATEMENT(", "").gsub(/[\);\n]/, ""), file, num
		elsif line.start_with? "//FUNCTION("
			args = line.gsub("//FUNCTION(", "").gsub(/[\);\n ]/, "").split(",")
			name = args[0]
			count = args.length > 0 ? args[1] : 0
  		end
	end
	func_str
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
