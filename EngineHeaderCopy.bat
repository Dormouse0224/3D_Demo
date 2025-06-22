del /s /q ".\External\Include\Engine"
xcopy /s /y /exclude:exclude_list.txt ".\Project\Engine\*.h" ".\External\Include\Engine"
xcopy /s /y /exclude:exclude_list.txt ".\Project\Engine\*.inl" ".\External\Include\Engine"

del /s /q ".\Output\Content\HLSL\Engine"
xcopy /s /y /exclude:exclude_list.txt ".\Project\Engine\*.fx" ".\Output\Content\HLSL\Engine"