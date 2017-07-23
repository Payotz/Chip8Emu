SET include_path=D:\Payotz\dev\include
SET lib_path=D:\Payotz\dev\lib

cl main.cpp -I %include_path% SDL2.lib SDL2_mixer.lib /link -LIBPATH:%lib_path%