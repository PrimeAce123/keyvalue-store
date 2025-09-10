.PHONY: all clean

all: release debug

# Add any new CMAKE_VARS here
CMAKE_VARS:= -DDEBUG_LOGGING=1 $\
			 -DDEBUG_PROFILING=1


# If you want to conditionally add a CMAKE_VAR, use the following format
# You can add them by doing
#
#     VARIABLE=<whatever value you want> make [debug] [release]
#
# ifeq (${<VARIABLE>}, 1)
#	 CMAKE_VARS:=${CMAKE_VARS} -DVARIABLE=1
# else
#	 CMAKE_VARS:=${CMAKE_VARS} -DVARIABLE=0
# endif

release:
	@echo "Building with following CMAKE_VARS = $(CMAKE_VARS)"
	mkdir -p ./build/release && \
	cd build/release && \
	cmake ${CMAKE_VARS} ${GUEST_DB_VARS} -DCMAKE_BUILD_TYPE=Release ../.. && \
	cmake --build . --config Release -j

debug:
	@echo "Building with following CMAKE_VARS = $(CMAKE_VARS)"
	mkdir -p ./build/debug && \
	cd build/debug && \
	cmake ${CMAKE_VARS} ${GUEST_DB_VARS} -DCMAKE_BUILD_TYPE=Debug ../.. && \
	cmake --build . --config Debug -j

clean:
	rm -rf build
