SET include_path=
SET lib_path=

cl main.cpp -I %include_path% SDL2.lib SDL2_image.lib /link -LIBPATH:%lib_path%