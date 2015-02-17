#! /usr/bin/ruby

require 'pp'

$dir = File.expand_path File.dirname(__FILE__)

$files = `echo #{$dir}/*.mvm`.split(" ").map {|s| s.chomp}

$errors = 0

def test file
	exp_file = file.gsub(".mvm", ".out")
	fail_file = file.gsub(".mvm", ".fail")
	res = `#{$dir}/../mvm < #{file}`
	if File.exists?(fail_file)
		if $?.exitstatus > 0
			puts "\032[0;31Test #{file}\033[0m"
		else
			puts "\033[0;31mTest #{file} (expected exception)\033[0m"
			$errors += 1
		end
	else
		if $?.exitstatus > 0
			puts "\033[0;31mTest #{file} (expected no direct exception)\033[0m"
			$errors += 1
			return
		end
		escaped = res.gsub('\\', '\\\\').gsub('"', '\\"').gsub("'", "\\'")
		diff = `bash -c 'colordiff -u10000 #{exp_file} <(echo "#{escaped}")'`
		if $?.exitstatus > 0
			puts "\033[0;31mTest #{file}\033[0m"
			system("cat #{file}")
			puts "----------------"
			puts diff
			$errors += 1
		else
			puts "\033[0;32mTest #{file}\033[0m"
		end
	end
end

$files.each do |file|
	test file
end

puts "#{$errors > 0 ? "\033[0;31m" : "\033[0;32m"}Failing number of test: #{$errors}, Overall number of tests: #{$files.length - $errors}\033[0m"
if $errors > 0
	exit 1
end
