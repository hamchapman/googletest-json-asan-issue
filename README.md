# Bug reproduction

## Overview

The project uses CMake to build a library, `FooBug`, and a `tests` executable,
which is what is making use of googletest.

There appears to be an issue with googletest and an interaction with:

* defining at least 5 tests
* compiling the `tests` executable, which links `gtest_main` with the following
  ASAN-related flag: `-fsanitize=address`
* making use of [nlohmann/json](https://github.com/nlohmann/json), specifically
  creating a `nlohmann::json` object and then trying to extract the value using
  the `get<std::vector<int>>()` syntax, for example:

  ```cpp
  json ints = json{1, 2, 3};
  std::vector<int> ints_vec = ints.get<std::vector<int>>();
  ```

This reproduction is setup to [enable the address sanitizer](https://github.com/hamchapman/googletest-json-asan-issue/blob/43938546ab3c79a05dbc3fe385dadaee625445f2/CMakeLists.txt#L52-L58). If you don't enable ASAN then you'll end up with a runtime segfault instead of the link time error.

## How to reproduce issue

Clone the repo and run the following to reproduce the issue:

```
mkdir -p build && cd build && cmake .. && make -j8 && ./tests
```

You'll then see some output like the following.

<details>
<summary>Error output</summary>
<pre><code>
-- Configuring done
-- Generating done
-- Build files have been written to: /Users/ham/Desktop/cpp/build/googletest-download
[ 11%] Performing update step for 'googletest'
[ 22%] No configure step for 'googletest'
[ 33%] No build step for 'googletest'
[ 44%] No install step for 'googletest'
[ 55%] No test step for 'googletest'
[ 66%] Completed 'googletest'
[100%] Built target googletest
-- Configuring done
-- Generating done
-- Build files have been written to: /Users/ham/Desktop/cpp/build
[ 50%] Built target FooBug
[ 50%] Built target gtest
[ 75%] Built target gtest_main
Scanning dependencies of target tests
[ 87%] Building CXX object CMakeFiles/tests.dir/test/FooTest.cpp.o
[100%] Linking CXX executable tests
=================================================================
==28828==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x6020000002bc at pc 0x0001085d2ba3 bp 0x7ffee7893e40 sp 0x7ffee7893608
READ of size 16 at 0x6020000002bc thread T0
    #0 0x1085d2ba2 in __asan_memcpy (libclang_rt.asan_osx_dynamic.dylib:x86_64h+0x43ba2)
    #1 0x10838efdf in std::__1::enable_if<((std::__1::integral_constant<bool, true>::value) || (!(__has_construct<std::__1::allocator<int>, bool*, bool>::value))) && (is_trivially_move_constructible<bool>::value), void>::type std::__1::allocator_traits<std::__1::allocator<int> >::__construct_backward<int>(std::__1::allocator<int>&, bool*, bool*, bool*&) memory:1699
    #2 0x10838e570 in std::__1::vector<int, std::__1::allocator<int> >::__swap_out_circular_buffer(std::__1::__split_buffer<int, std::__1::allocator<int>&>&) vector:937
    #3 0x1083e3acb in void std::__1::vector<int, std::__1::allocator<int> >::__push_back_slow_path<int>(int&&) vector:1621
    #4 0x1083aacd9 in std::__1::vector<int, std::__1::allocator<int> >::push_back(int&&) vector:1658
    #5 0x1083aab5e in testing::TestSuite::AddTestInfo(testing::TestInfo*) gtest.cc:2994
    #6 0x1083a95d3 in testing::internal::UnitTestImpl::AddTestInfo(void (*)(), void (*)(), testing::TestInfo*) gtest-internal-inl.h:700
    #7 0x1083a93c3 in testing::internal::MakeAndRegisterTestInfo(char const*, char const*, char const*, char const*, testing::internal::CodeLocation, void const*, void (*)(), void (*)(), testing::internal::TestFactoryBase*) gtest.cc:2769
    #8 0x10839985f in __cxx_global_var_init.9 FooTest.cpp:18
    #9 0x108399a58 in _GLOBAL__sub_I_FooTest.cpp FooTest.cpp
    #10 0x1139491e2 in ImageLoaderMachO::doModInitFunctions(ImageLoader::LinkContext const&) (dyld:x86_64+0x1b1e2)
    #11 0x1139495ed in ImageLoaderMachO::doInitialization(ImageLoader::LinkContext const&) (dyld:x86_64+0x1b5ed)
    #12 0x11394400a in ImageLoader::recursiveInitialization(ImageLoader::LinkContext const&, unsigned int, char const*, ImageLoader::InitializerTimingList&, ImageLoader::UninitedUpwards&) (dyld:x86_64+0x1600a)
    #13 0x113942013 in ImageLoader::processInitializers(ImageLoader::LinkContext const&, unsigned int, ImageLoader::InitializerTimingList&, ImageLoader::UninitedUpwards&) (dyld:x86_64+0x14013)
    #14 0x1139420b3 in ImageLoader::runInitializers(ImageLoader::LinkContext const&, ImageLoader::InitializerTimingList&) (dyld:x86_64+0x140b3)
    #15 0x1139305e5 in dyld::initializeMainExecutable() (dyld:x86_64+0x25e5)
    #16 0x113935af7 in dyld::_main(macho_header const*, unsigned long, int, char const**, char const**, char const**, unsigned long*) (dyld:x86_64+0x7af7)
    #17 0x11392f226 in dyldbootstrap::start(dyld3::MachOLoaded const*, int, char const**, dyld3::MachOLoaded const*, unsigned long*) (dyld:x86_64+0x1226)
    #18 0x11392f024 in _dyld_start (dyld:x86_64+0x1024)

0x6020000002c0 is located 0 bytes to the right of 16-byte region [0x6020000002b0,0x6020000002c0)
allocated by thread T0 here:
    #0 0x1085e0fdd in wrap__Znwm (libclang_rt.asan_osx_dynamic.dylib:x86_64h+0x51fdd)
    #1 0x108376648 in std::__1::__libcpp_allocate(unsigned long, unsigned long) new:253
    #2 0x10838ed67 in std::__1::allocator<int>::allocate(unsigned long, void const*) memory:1813
    #3 0x10838ebc0 in std::__1::allocator_traits<std::__1::allocator<int> >::allocate(std::__1::allocator<int>&, unsigned long) memory:1546
    #4 0x10838e96f in std::__1::__split_buffer<int, std::__1::allocator<int>&>::__split_buffer(unsigned long, unsigned long, std::__1::allocator<int>&) __split_buffer:311
    #5 0x10838e49c in std::__1::__split_buffer<int, std::__1::allocator<int>&>::__split_buffer(unsigned long, unsigned long, std::__1::allocator<int>&) __split_buffer:310
    #6 0x1083e3a7c in void std::__1::vector<int, std::__1::allocator<int> >::__push_back_slow_path<int>(int&&) vector:1617
    #7 0x1083aacd9 in std::__1::vector<int, std::__1::allocator<int> >::push_back(int&&) vector:1658
    #8 0x1083aab5e in testing::TestSuite::AddTestInfo(testing::TestInfo*) gtest.cc:2994
    #9 0x1083a95d3 in testing::internal::UnitTestImpl::AddTestInfo(void (*)(), void (*)(), testing::TestInfo*) gtest-internal-inl.h:700
    #10 0x1083a93c3 in testing::internal::MakeAndRegisterTestInfo(char const*, char const*, char const*, char const*, testing::internal::CodeLocation, void const*, void (*)(), void (*)(), testing::internal::TestFactoryBase*) gtest.cc:2769
    #11 0x1083990ff in __cxx_global_var_init.5 FooTest.cpp:13
    #12 0x108399a4e in _GLOBAL__sub_I_FooTest.cpp FooTest.cpp
    #13 0x1139491e2 in ImageLoaderMachO::doModInitFunctions(ImageLoader::LinkContext const&) (dyld:x86_64+0x1b1e2)
    #14 0x1139495ed in ImageLoaderMachO::doInitialization(ImageLoader::LinkContext const&) (dyld:x86_64+0x1b5ed)
    #15 0x11394400a in ImageLoader::recursiveInitialization(ImageLoader::LinkContext const&, unsigned int, char const*, ImageLoader::InitializerTimingList&, ImageLoader::UninitedUpwards&) (dyld:x86_64+0x1600a)
    #16 0x113942013 in ImageLoader::processInitializers(ImageLoader::LinkContext const&, unsigned int, ImageLoader::InitializerTimingList&, ImageLoader::UninitedUpwards&) (dyld:x86_64+0x14013)
    #17 0x1139420b3 in ImageLoader::runInitializers(ImageLoader::LinkContext const&, ImageLoader::InitializerTimingList&) (dyld:x86_64+0x140b3)
    #18 0x1139305e5 in dyld::initializeMainExecutable() (dyld:x86_64+0x25e5)
    #19 0x113935af7 in dyld::_main(macho_header const*, unsigned long, int, char const**, char const**, char const**, unsigned long*) (dyld:x86_64+0x7af7)
    #20 0x11392f226 in dyldbootstrap::start(dyld3::MachOLoaded const*, int, char const**, dyld3::MachOLoaded const*, unsigned long*) (dyld:x86_64+0x1226)
    #21 0x11392f024 in _dyld_start (dyld:x86_64+0x1024)

SUMMARY: AddressSanitizer: heap-buffer-overflow (libclang_rt.asan_osx_dynamic.dylib:x86_64h+0x43ba2) in __asan_memcpy
Shadow bytes around the buggy address:
  0x1c0400000000: fa fa fd fd fa fa 00 00 fa fa 00 03 fa fa 00 02
  0x1c0400000010: fa fa 00 04 fa fa 00 00 fa fa 00 06 fa fa 00 fa
  0x1c0400000020: fa fa 00 00 fa fa 00 fa fa fa 00 fa fa fa 00 fa
  0x1c0400000030: fa fa 00 fa fa fa 00 fa fa fa 04 fa fa fa fd fa
  0x1c0400000040: fa fa fd fa fa fa 00 fa fa fa fd fd fa fa fd fa
=>0x1c0400000050: fa fa 00 fa fa fa 00[04]fa fa 00 fa fa fa 00 fa
  0x1c0400000060: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c0400000070: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c0400000080: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c0400000090: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x1c04000000a0: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
  Shadow gap:              cc
==28828==ABORTING
CMake Error at /usr/local/Cellar/cmake/3.17.2/share/cmake/Modules/GoogleTestAddTests.cmake:40 (message):
  Error running test executable.

    Path: '/Users/ham/Desktop/cpp/build/tests'
    Result: Child aborted
    Output:




make[2]: *** [tests] Error 1
make[2]: *** Deleting file `tests'
make[1]: *** [CMakeFiles/tests.dir/all] Error 2
make: *** [all] Error 2
</code></pre>
</details>

## Specifics

As written out in comments in the [`Footest.cpp` file](https://github.com/hamchapman/googletest-json-asan-issue/blob/master/test/FooTest.cpp), you can stop the issue from occurring if you do at least one of the following:

1. Comment out the line that adds the `-fsanitize=address` compiler flag for the `tests` executable.
2. Comment out at least one of the test cases so that only 4 (or fewer) remain in total.
3. Comment out the line in the tests that tries to get the json value as a
   vector of `int`s.

You can see in [`Footest.cpp`](https://github.com/hamchapman/googletest-json-asan-issue/blob/master/test/FooTest.cpp) that using nlohmann/json with a vector of `double`s is fine. It's only when a vector of `int`s are used that the heap buffer overflow occurs.

## Related issues?

The only thing I could find that seems at all similar is this issue: https://github.com/google/googletest/issues/487
