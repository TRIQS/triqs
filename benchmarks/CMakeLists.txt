# The list of benchs
file(GLOB_RECURSE all_benchs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

include(FetchContent)

# NB: google-benchmark (benchmark_main) is already provided by triqs itself
# (deps/CMakeLists.txt, gated on Build_Benchs) — do not fetch it again here.

set(BAOBZI_BUILD_STATIC ON)
set(BAOBZI_CPU_DISPATCH OFF)
# Fetch the baobzi library
FetchContent_Declare(
        baobzi
        GIT_REPOSITORY https://github.com/flatironinstitute/baobzi.git
        GIT_TAG main  # You can specify a specific commit, branch, or tag
)
FetchContent_MakeAvailable(baobzi)
unset(BAOBZI_BUILD_STATIC)
unset(BAOBZI_CPU_DISPATCH)

# Fetch treeweave (header-only; pulls polyfit/POET transitively) for comparison
FetchContent_Declare(
        treeweave
        GIT_REPOSITORY https://github.com/DiamonDinoia/treeweave.git
        GIT_TAG main
)
FetchContent_MakeAvailable(treeweave)

foreach (bench ${all_benchs})
    get_filename_component(bench_name ${bench} NAME_WE)
    get_filename_component(bench_dir ${bench} DIRECTORY)
    add_executable(${bench_name} ${bench})
    target_link_libraries(${bench_name} triqs triqs_warnings benchmark_main baobzi treeweave::treeweave)
    set_property(TARGET ${bench_name} PROPERTY RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${bench_dir})
    set_target_properties(${bench_name} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE CXX_STANDARD 20 CXX_STANDARD_REQUIRED ON CXX_EXTENSIONS OFF)
    #add_bench(NAME ${bench_name} COMMAND ${bench_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${bench_dir})
    target_compile_options(${bench_name} PRIVATE -march=native -O3 -ftree-vectorize -fcx-fortran-rules -fcx-limited-range -ffp-contract=fast)
    # Run clang-tidy if found
    if (CLANG_TIDY_EXECUTABLE)
        set_target_properties(${bench_name} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE}")
    endif ()
    # Run cppcheck if found
    if (CPPCHECK_EXECUTABLE)
        add_custom_command(
                TARGET ${bench_name}
                COMMAND ${CPPCHECK_EXECUTABLE}
                --enable=warning,style,performance,portability
                --std=c++20
                --template=gcc
                --verbose
                --force
                --quiet
                ${CMAKE_CURRENT_SOURCE_DIR}/${bench}
        )
    endif ()
endforeach ()
