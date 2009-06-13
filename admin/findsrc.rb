#!/usr/bin/ruby
require 'find'
require "#{File.dirname __FILE__}/platform"

def findsrc dir='.'
  excludes = ['.svn','.git','_include','tests','_build']
  case Platform::IMPL
    when :macosx then excludes<<'win'
    when :mswin, :cygwin then excludes<<'mac'
    else excludes<<'win'<<'mac'
  end
  
  Find.find dir do |path|
    next if path == dir # oddly neccessary
    path.sub! %r[^\./], ''
    if File.directory? path
      Find.prune if excludes.include? File.basename(path)
      #don't recurse into dirs with pro files in
      Find.prune if Dir["#{path}/*.pro"].length > 0 and $findsrc_prune_pro
    elsif File.file? path
      case Platform::IMPL
        when :macosx then next if /_mac\.cpp$/.match path
        when :mswin, :cygwin then next if /_win\.cpp$/.match path
      end
      yield( path, File.extname( path ) ) unless File.basename(path) == 'EXAMPLE.cpp'
    end
  end
end

if $0 == __FILE__
  extnames=ARGV.collect {|extname| ".#{extname}"}
  findsrc {|path,extname| puts path if extnames.include? extname}
end