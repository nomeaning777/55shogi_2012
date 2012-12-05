# -*- coding:utf-8 -*-
require 'rake/clean'

CPP = 'g++'
CPP_FLAGS = '-std=gnu++0x -Wall'
CPP_DEBUG_FLAGS = '-pg -g -O0 -fmessage-length=0'
CPP_RELEASE_FLAGS = '-O2 -mtune=amdfam10 -mabm -msse4a'

LIBS = ['boost_system', 'readline', 'pthread']
SRC = FileList['*.cpp']
OBJ = SRC.ext('o')
CLEAN.include('*.o')
CLEAN.include('a.out', 'documents', 'gmon.out')
CLOBBER.include('55shogi')

task :default => '55shogi'
task :debug => '.debug'
task :release => '.release'
desc 'コンパイルして実行します'
task :run => '55shogi' do
  if File.exist?('.mpi')
    sh "mpirun -np 5 ./55shogi"
  else
    sh "./55shogi"
  end
end

task :doc do
  sh "doxygen 55shogi.doxyfile"
end

file '.debug' do
  Rake::Task['clean'].invoke
  sh 'rm -f ./.release'
  sh 'touch ./.debug'
  Rake::Task['default'].invoke
end

file '.release' do
  Rake::Task['clean'].invoke
  sh 'rm -f ./.debug'
  sh 'touch ./.release'
  Rake::Task['default'].invoke
end

# 現在のモードを元にフラグを確定する
def cpp_mode_flags
  mode = 'debug'
  if File.exist?('.release')
    mode = 'release'
  end
  res = eval("CPP_#{mode.upcase}_FLAGS")
  if File.exist?('.mpi')
    res += ' -DMPI_ENABLE'
  end
  return res
end

rule '.o' => '.cpp' do |t|
  sh "#{CPP} -c -o #{t.name} #{t.source} #{LIBS.map{|a| "-l#{a}"}.join(' ')} #{CPP_FLAGS} #{cpp_mode_flags}"  
end

file '55shogi' => OBJ do
  sh "#{CPP} -o 55shogi #{OBJ} #{LIBS.map{|a| "-l#{a}"}.join(' ')} #{CPP_FLAGS} #{cpp_mode_flags}"
end

