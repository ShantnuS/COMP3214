--
-- When cross building use --os=linux
--
-- A solution contains projects, and defines the available configurations
--
-- Version 8c
--
print('This generates project and files for RGL.')

-- build  directory.
function make (target)  -- configures a build target from lists of files
    for i, n in ipairs(target) do
        project(n[1])
            print('Target is ' .. n[1])
            targetname(n[1])
            for j, nn in ipairs(n) do
                if j > 1 then
                    files(nn)
                    print('Compiling ' .. nn)
                end
            end
            kind 'ConsoleApp'
            links{'RGL'}
            libdirs{'./lib'}
            buildoptions{"-std=c++11"}
            if SDL == 12 then
                if os.get() == 'windows' then
                    links{'mingw32'}
                end
                links{'SDLmain'}
                links{'SDL'}
            elseif SDL == 20 then
                 if os.get() == 'windows' then
                    links{'mingw32'}
                end
                links{'SDL2main'}
                links{'SDL2'}
            else
                print('SDL version not supported', SDL)
            end
            targetdir '.'
    end
end

-- Build Library
solution 'BuildRGL'
    root =  _ACTION
    configurations { 'Release' }
        language 'C++'
        includedirs{ './include'}
        libdirs{'./lib'}
        buildoptions{'-fmax-errors=1'}
        buildoptions{'-Wno-write-strings'}
        buildoptions{"-std=c++11"}
        -- not tested on RPi 2015/2016/2017
        if os.get() == 'linux' then -- Need this for RPi and Linux.
            print 'Building for linux on a Raspberry Pi, nothing else'
            includedirs{ '/usr/local/include'}
            includedirs{'./include'}
            libdirs{'/usr/lib/arm-linux-gnueabihf'} -- fix for true linux.
            defines{'RASPBERRY_PI'}
--~             defines{'WIDTH=320', 'HEIGHT=240'}
            defines('SDL12')
            defines('_GNU_SOURCE=1', '_REENTRANT')
            links{'SDL'}
            SDL = 12
        end
        if os.get() == 'windows' then
            print 'Building for windows' -- Build for Windows, using SDL 1.2 or SDL 2.x (currently x is 0)
            SDL =  12 -- chose between '12' and '20'
--~             SDL =  20 -- '12' or '20'
            print('SDL Version', SDL)
            if SDL == 12 then
                print('Building SDL 1.2')
                print('SDL 1.2 buffers "stdout", "stderr" to files of those names. If you want interactive debuging use SDL 2.0')
                defines{'SDL12'}
            elseif SDL == 20 then
                print('Building SDL 2.0')
                defines{'SDL20'}
                print('Ignore this: Warning: corrupt .drectve at end of def file')
            end
        end

    -- A project defines one build target, a link library or an executable

    project 'RGL'
          location ("./build/" .. root .. '/RGLlibrary/')
          kind 'StaticLib'
          targetdir './lib'
        files { 'include/*.h',  'rgl/*.cpp'}
        excludes{'_*.cpp'}
        if SDL == 12 then
            excludes{'rgl/rgl2d-sdl20.cpp'} -- code based on SDL 2
        elseif SDL == 20 then
            excludes{'rgl/rgl2d-sdl12.cpp'} -- code based on SDL 1.2
        else
            print('SDL version not supported', SDL)
        end
        targetdir './lib'
--
-- edit the following lines to specify a list of files to build.
--
-- First entry on each line is the name of the project/executable
-- The remaining are the names of the files to be built
-- This builds
--          rendering triangles.
--
    targets = {
                   { 'test_Triangles', './Examples/test_Triangles.cpp'},
                   { 'test_Shaded_Triangles', './Examples/test_Shaded_Triangles.cpp'},
                   { 'test_Wire_Cube', './Examples/wireCube.cpp'},
                   }
    print('Building targets')
    make(targets)



    configuration 'Release'
        defines { 'NDEBUG' }
        flags { 'Optimize' }
