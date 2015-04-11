#! /usr/bin/ruby

require 'pp'

$dir = File.expand_path File.dirname(__FILE__)


$success_files = `echo #{$dir}/*.success`.split(" ").map {|s| s.chomp}
$fail_files = `echo #{$dir}/*.fail`.split(" ").map {|s| s.chomp}

$errors = 0

def test file
	should_succeed = file.end_with? "success"
	res = `#{$dir}/../../mvm parse < #{file} 2>&1`
	if should_succeed ^ ($?.exitstatus > 0)
		puts "\033[0;32mTest #{file}\033[0m"
	else
		puts "\033[0;31mTest #{file}: #{res}\033[0m"
		$errors += 1
	end
end

($success_files + $fail_files).each do |file|
	test file
end

puts "#{$errors > 0 ? "\033[0;31m" : "\033[0;32m"}Failing number of test: #{$errors}, Overall number of tests: #{$success_files.length + $fail_files.length}\033[0m"
if $errors > 0
	exit 1
end
