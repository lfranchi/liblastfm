#!/usr/bin/ruby
# tool to look through a qmake pro file and find what headers are to be 
# installed it then figures out the classes in each of those files
# and then creates the directory _include and creates files named after the 
# class names in there which #include the file that contains the class

cwd = File.dirname( __FILE__ )
require 'find'
require 'ftools'
require "#{cwd}/platform.rb"


case Platform::IMPL
  when :mswin
    $cp='ruby -e "require \'FileUtils\'; FileUtils.copy_file(ARGV[0], ARGV[1])" --'
    $ln=$cp
    $mkdir='ruby -e "require \'FileUtils\'; FileUtils.mkpath ARGV[0]" --'
    $orderonly=''
  else
    $cp='cp'
    $ln='cp' #cp doesn't work for some reason, the target is always remade
    $mkdir='mkdir -p'
    $orderonly='|'
end

def step3( path, classname )
puts <<-EOS
_include/lastfm/#{classname}: #{path} #{$orderonly} _include/lastfm
	#{$ln} #{path} $@
$(DESTDIR)#{$install_prefix}/include/lastfm/#{classname}: #{path} #{$orderonly} $(DESTDIR)#{$install_prefix}/include/lastfm
	#{$cp} #{path} $@

EOS

  $headers << "_include/lastfm/#{classname}"
  $installheaders << "$(DESTDIR)#{$install_prefix}/include/lastfm/#{classname}"
end

def step2( path )
  b = nil
  File.open( path ).each_line do |line|
    matches = /(class|struct)\s*LASTFM_DLLEXPORT\s*([a-zA-Z0-9]+)/.match( line )
    if !matches.nil?
      b = true
      step3( path, matches[2] )
    end
  end
  # otherwise just copy it without adjustment
  step3( path, File.basename( path ) ) if b.nil?
end
################################################################################


$install_prefix = ENV['PREFIX']
exit 1 if $install_prefix.nil?

$installheaders = Array.new
$headers = Array.new

ARGV.each { |h| step2( 'src/'+h ) }

puts <<-EOS
_include/lastfm:
	#{$mkdir} $@
$(DESTDIR)#{$install_prefix}/include/lastfm:
	#{$mkdir} $@

_include/lastfm.h: #{$headers.join(' ')} #{$orderonly} _include/lastfm
	ruby admin/lastfm_h.rb $@
$(DESTDIR)#{$install_prefix}/include/lastfm.h: _include/lastfm.h #{$orderonly} $(DESTDIR)#{$install_prefix}/include/lastfm
	#{$cp} _include/lastfm.h $@

.PHONY: headers
headers: #{$headers.join(' ')} _include/lastfm.h

.PHONY: install
install: #{$installheaders.join(' ')} $(DESTDIR)#{$install_prefix}/include/lastfm.h
	cd src && make install "INSTALL_ROOT=$(DESTDIR)#{$install_prefix}"

.PHONY: clean
clean:
	rm -rf _include
	rm -rf src/_build
	rm -rf demos/_build
	rm -rf tests/_build
	rm -f src/Makefile
	rm -f tests/Makefile
	rm -f demos/Makefile
	rm -rf _bin

.PHONY: distclean
distclean: clean
	rm -f src/_files.qmake
	rm -f src/_version.h
	rm -f Makefile

EOS
