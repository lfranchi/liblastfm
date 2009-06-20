#!/usr/bin/ruby
# tool to look through a qmake pro file and find what headers are to be 
# installed it then figures out the classes in each of those files
# and then creates the directory _include and creates files named after the 
# class names in there which #include the file that contains the class

cwd = File.dirname( __FILE__ )
require 'find'
require "#{cwd}/platform.rb"


######################################################################### defs
case Platform::IMPL
  when :mswin
    CP='ruby -e "require \'FileUtils\'; FileUtils.copy_file(ARGV[0], ARGV[1])" --'
    LN=CP
	RMF='ruby -e "require \'FileUtils\'; FileUtils.rm(ARGV[0], :force => true)" --'
	RMRF='ruby -e "require \'FileUtils\'; FileUtils.rm_rf ARGV[0]" --'	
    MKDIR='ruby -e "require \'FileUtils\'; FileUtils.mkpath ARGV[0]" --'
    ORDERONLY=''
  else
    CP='cp'
    LN='cp' #'ln -sf' oddly doesn't work, the target is always remade
	RMF='rm -f'
	RMRF='rm -rf'	
    MKDIR='mkdir -p'
    ORDERONLY='|'
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
INSTALL_PREFIX=ENV['LFM_PREFIX']
abort("Environment variable LFM_PREFIX not defined") if INSTALL_PREFIX.nil?
$install_headers=''
$headers=''

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
	#{RMRF} _include
	#{RMRF} src/_build
	#{RMRF} src/fingerprint/_build
	#{RMRF} demos/_build
	#{RMRF} tests/_build
	#{RMF} src/Makefile
	#{RMF} src/fingerprint/Makefile
	#{RMF} tests/Makefile
	#{RMF} demos/Makefile
	#{RMRF} _bin

.PHONY: distclean
distclean: clean
	#{RMF} .qmake.env
	#{RMF} src/_files.qmake
	#{RMF} src/_version.h
	#{RMF} src/fingerprint/_files.qmake
	#{RMF} src/fingerprint/_version.h
	#{RMF} Makefile

EOS

begin
  lhd='_include/lastfm'
  ihd="$(DESTDIR)#{INSTALL_PREFIX}/include/lastfm"
  ARGV.each do |header|
    penis "src/#{header}" do |path, classname|
      puts "#{lhd}/#{classname}: #{path} #{ORDERONLY} #{lhd}"
      puts "	#{LN} #{path} $@"
      puts "#{ihd}/#{classname}: #{path} #{ORDERONLY} #{ihd}"
      puts "	#{CP} #{path} $@"
      puts
      $headers+=" #{lhd}/#{classname}"
      $install_headers+=" #{ihd}/#{classname}"
    end
  end
end

puts <<-EOS
_include/lastfm:
	#{MKDIR} $@
$(DESTDIR)#{INSTALL_PREFIX}/include/lastfm:
	#{MKDIR} $@

_include/lastfm.h: #{$headers} #{ORDERONLY} _include/lastfm
	ruby admin/lastfm.h.rb $@
$(DESTDIR)#{INSTALL_PREFIX}/include/lastfm.h: _include/lastfm.h #{ORDERONLY} $(DESTDIR)#{INSTALL_PREFIX}/include/lastfm
	#{CP} _include/lastfm.h $@

.PHONY: headers
headers: #{$headers} _include/lastfm.h

.PHONY: install
install: #{$install_headers} $(DESTDIR)#{INSTALL_PREFIX}/include/lastfm.h
EOS

if Platform::IMPL == :macosx
  # qmake doesn't do the install_name_tool steps, so we have to do everything
  dst="$(DESTDIR)#{INSTALL_PREFIX}/lib"
  v=ENV['LFM_VERSION']
  vmajor=v[0..0]
  puts "	mkdir -p #{dst}"
  ['liblastfm', 'liblastfm_fingerprint'].each do |base|
    puts "	cp _bin/#{base}.#{v}.dylib #{dst}"
    puts "	cd #{dst} && ln -s #{base}.#{v}.dylib #{base}.dylib"
    puts "	cd #{dst} && ln -s #{base}.#{v}.dylib #{base}.#{vmajor}.dylib"
    puts "	install_name_tool -id #{dst}/#{base}.#{vmajor}.dylib #{dst}/#{base}.#{vmajor}.dylib"
  end
  ext="#{vmajor}.dylib"
  puts "	install_name_tool -change liblastfm.#{ext} #{dst}/liblastfm.#{ext} #{dst}/liblastfm_fingerprint.#{ext}"
else 
  puts %Q[	cd src && $(MAKE) install "INSTALL_ROOT=$(DESTDIR)#{INSTALL_PREFIX}"]
  puts %Q[	cd src/fingerprint && $(MAKE) install "INSTALL_ROOT=$(DESTDIR)#{INSTALL_PREFIX}"]
end

BASENAME='liblastfm-'+ENV['LFM_VERSION']
puts <<-EOS

.PHONY: dist
dist:
	git archive --prefix=#{BASENAME}/ HEAD | bzip2 > #{BASENAME}.tar.bz2
EOS