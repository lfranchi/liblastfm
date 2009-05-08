cwd = File.dirname( __FILE__ )
require "#{cwd}/platform.rb"

def h(s, n)
  case Platform::IMPL
  when :mswin
    puts '==>'+s
  else
    puts "\033[0;#{n}m==>\033[0;0;1m #{s} \033[0;0m"
  end
end

def h1 s
    h(s, 34)
end

def h2 s
    h(s, 33)
    yield
end
