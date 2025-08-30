all:
	make clean
	make configure
	make preBuildCoverage
	make buildTests
	make coverage

clean:
	rm -rf build 
	rm -rf TestResults

configure:
	cmake -B build -DUSE_CODE_COVERAGE=ON -DCOVERAGE_VERBOSE=ON

preBuildCoverage:
	cmake --build build --target forceCover -j 12
	cmake --build build --target forcecover_prep -j 12

buildTests:
	cmake --build build -j 12

test:
	ctest --test-dir build --output-on-failure -j 12

coverage:
	cmake --build build --target cov -j 12