#!/usr/bin/ruby
# tool to look through a qmake pro file and find what headers are to be 
# installed it then figures out the classes in each of those files
# and then creates the directory _include and creates files named after the 
# class names in there which #include the file that contains the class

cwd = File.dirname( __FILE__ )
require 'find'
require 'ftools'
require "#{cwd}/platform.rb"


######################################################################### defs
case Platform::IMPL
  when :mswin
    $cp='ruby -e "require \'FileUtils\'; FileUtils.copy_file(ARGV[0], ARGV[1])" --'
    $ln=$cp
    $mkdir='ruby -e "require \'FileUtils\'; FileUtils.mkpath ARGV[0]" --'
    $orderonly=''
  else
    $cp='cp'
    $ln='cp' #'ln -sf' oddly doesn't work, the target is always remade
    $mkdir='mkdir -p'
    $orderonly='|'
end

def penis( path )
  yielded = false
  File.open( path ).each_line do |line|
    matches = /(class|struct)\s*LASTFM_DLLEXPORT\s*([a-zA-Z0-9]+)/.match( line )
    unless matches.nil?
      yield path, matches[2]
      yielded = true
    end
  end
  # just copy it without adjustment if there were no exported classes
  yield path, File.basename( path ) unless yielded
end


######################################################################### main
$install_headers=''
$headers=''
$install_prefix = ENV['LFM_PREFIX']
abort("Environment variable LFM_PREFIX not defined") if $install_prefix.nil?

puts <<-EOS
.PHONY: all
all: headers __src __fingerprint __demos __tests

.PHONY: __src
__src: src/Makefile
	cd src && $(MAKE)
.PHONY: __fingerprint
__fingerprint: src/fingerprint/Makefile __src
	cd src/fingerprint && $(MAKE)
.PHONY: __tests
__tests: tests/Makefile __src
	cd tests && $(MAKE)
.PHONY: __demos
__demos: demos/Makefile __src
	cd demos && $(MAKE)

src/Makefile:
	cd src && #{ENV['LFM_QMAKE']}
src/fingerprint/Makefile:
	cd src/fingerprint && #{ENV['LFM_QMAKE']}
tests/Makefile:
	cd tests && #{ENV['LFM_QMAKE']}
demos/Makefile:
	cd demos && #{ENV['LFM_QMAKE']}

.PHONY: clean
clean:
	rm -rf _include
	rm -rf src/_build
	rm -rf src/fingerprint/_build
	rm -rf demos/_build
	rm -rf tests/_build
	rm -f src/Makefile
	rm -f src/fingerprint/Makefile
	rm -f tests/Makefile
	rm -f demos/Makefile
	rm -rf _bin

.PHONY: distclean
distclean: clean
	rm -f .qmake.env
	rm -f src/_files.qmake
	rm -f src/_version.h
	rm -f src/fingerprint/_files.qmake
	rm -f src/fingerprint/_version.h
	rm -f Makefile

EOS

begin
  lhd='_include/lastfm'
  ihd="$(DESTDIR)#{$install_prefix}/include/lastfm"
  ARGV.each do |header|
    penis "src/#{header}" do |path, classname|
      puts "#{lhd}/#{classname}: #{path} #{$orderonly} #{lhd}"
      puts "	#{$ln} #{path} $@"
      puts "#{ihd}/#{classname}: #{path} #{$orderonly} #{ihd}"
      puts "	#{$cp} #{path} $@"
      puts
      $headers+=" #{lhd}/#{classname}"
      $install_headers+=" #{ihd}/#{classname}"
    end
  end
end

puts <<-EOS
_include/lastfm:
	#{$mkdir} $@
$(DESTDIR)#{$install_prefix}/include/lastfm:
	#{$mkdir} $@

_include/lastfm.h: #{$headers} #{$orderonly} _include/lastfm
	ruby admin/lastfm.h.rb $@
$(DESTDIR)#{$install_prefix}/include/lastfm.h: _include/lastfm.h #{$orderonly} $(DESTDIR)#{$install_prefix}/include/lastfm
	#{$cp} _include/lastfm.h $@

.PHONY: headers
headers: #{$headers} _include/lastfm.h

.PHONY: install
install: #{$install_headers} $(DESTDIR)#{$install_prefix}/include/lastfm.h
	cd src && make install "INSTALL_ROOT=$(DESTDIR)#{$install_prefix}"
	cd src/fingerprint && make install "INSTALL_ROOT=$(DESTDIR)#{$install_prefix}"

EOS