all:
	make clean
	make configure
	make preBuild
	make buildTests
	make coverage

clean:
	rm -rf build 
	rm -rf TestResults

configure:
	cmake -B build -DUSE_CODE_COVERAGE=ON -DCOVERAGE_VERBOSE=ON

preBuild:
	cmake --build build --target forcecover_prep -j 12

buildTests:

test:
	ctest --test-dir build --output-on-failure -j 12

coverage:
	cmake --build build --target cov -j 12