del /s /q ".\External\Include\Script"
xcopy /s /y /exclude:exclude_list.txt ".\Project\Script\*.h" ".\External\Include\Script"
xcopy /s /y /exclude:exclude_list.txt ".\Project\Script\*.inl" ".\External\Include\Script"

del /s /q ".\Output\Content\HLSL\Script"
xcopy /s /y /exclude:exclude_list.txt ".\Project\Script\*.fx" ".\Output\Content\HLSL\Script"