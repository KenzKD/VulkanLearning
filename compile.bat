forfiles /s /m *.vert /c "cmd /c %VULKAN_SDK%/Bin/glslc.exe @path -o @fname.vert.spv"
forfiles /s /m *.frag /c "cmd /c %VULKAN_SDK%/Bin/glslc.exe @path -o @fname.frag.spv"
pause