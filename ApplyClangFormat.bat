@ECHO OFF
FOR /R %%f IN (*.cpp, *.h, *.cs) DO (
	clang-format -i %%f
	echo %%f
)
PAUSE