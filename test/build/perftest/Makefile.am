# Copyright (c) 2025 Huawei Technologies Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Header file path of gtest and gmock
GTEST_DIR = ../../googletest-release-1.8.0/googletest
GMOCK_DIR = ../../googletest-release-1.8.0/googlemock

# Source file of function
KPGLIBC_modules_DIR = ../../..
TEST_modules_DIR = ../../

# Header files path
KPGLIBC_DIR += -I $(GTEST_DIR)/include/
KPGLIBC_DIR += -I $(GMOCK_DIR)/include/

KPGLIBC_DIR += -I $(TEST_modules_DIR)/common/include/
KPGLIBC_DIR += -I $(TEST_modules_DIR)/common/perftest/
KPGLIBC_DIR += -I $(KPGLIBC_modules_DIR)/include/

ifeq ($(rpm), y)
KPGLIBC_DIR += -I /usr/local/ksl/include/
CFLAGS += -DLIB_GLIBC_NAME=\"libkpglibc.so\" -DLIB_GLIBC_TIME_NAME=\"libkpglibc_time.so\"
CXXFLAGS += -DLIB_GLIBC_NAME=\"libkpglibc.so\" -DLIB_GLIBC_TIME_NAME=\"libkpglibc_time.so\"
else
KPGLIBC_DIR += -I $(KPGLIBC_modules_DIR)/include/
CFLAGS += -DLIB_GLIBC_NAME=\"libkpglibc_sve.so\" -DLIB_GLIBC_TIME_NAME=\"libkpglibc_time.so\"
CXXFLAGS += -DLIB_GLIBC_NAME=\"libkpglibc_sve.so\" -DLIB_GLIBC_TIME_NAME=\"libkpglibc_time.so\"
endif

# Options for preprocessing stage, here are mainly header files.
CPPFLAGS += $(KPGLIBC_DIR)

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread -std=c++17 -O2
CFLAGS +=

# Test Source Code files
SRC_FILES_DIRS += $(TEST_modules_DIR)/common/perftest/

# Static and shared libraries that need to be linked to the program.
ifeq ($(rpm), y)
LD_LIBRARIES := -L$(TEST_modules_DIR)/test_tool_bins/gtest -L/usr/local/ksl/lib/ -Wl,-rpath /usr/local/ksl/lib/ -Wl,-rpath /usr/local/lib64/ -L/usr/local/lib64/
else
LD_LIBRARIES := -L$(TEST_modules_DIR)/test_tool_bins/gtest -L$(KPGLIBC_modules_DIR)/output/ -Wl,-rpath $(KPGLIBC_modules_DIR)/output/ -L/usr/local/lib64/
endif
LD_LIBRARIES += -pthread -lgtest -lgtest_main -lgmock -lgmock_main

# The name of the compiled executable file.
PROGRAM = kpglibc_perftest

# The directories in which source files reside. At least one path should be specified.
ifndef SILENCE
	SILENCE = @
endif

# C/C++ program
SRCEXTS   := .c .cc .cpp

SOURCES_MUD = $(foreach d,$(SRC_FILES_DIRS),$(wildcard $(addprefix $(d)/*,$(SRCEXTS))))
SOURCES = $(shell echo $(SOURCES_MUD) | \
			awk -F ' ' '{for(i=1;i<=NF;i++)a[i]=$$i}END{for(i=1;i<=asort(a);i++)printf a[i]" "}')

OBJS    = $(foreach x,$(SRCEXTS), \
      $(patsubst %$(x), %.o, $(filter %$(x),$(SOURCES))))

# The header dependence list
SRC_DEPS = $(foreach x,$(SRCEXTS), \
      $(patsubst %$(x), %.o.d, $(filter %$(x),$(SOURCES))))

# Header dependence flag
DEPS_FLAG = -MD -MF $@.d

CC_COMPILE = $(SILENCE) $(CC) $(DEPS_FLAG) -c $(CPPFLAGS) $(CFLAGS) -o $@ $<
CXX_COMPILE = $(SILENCE) $(CXX) $(DEPS_FLAG) -c $(CPPFLAGS) $(CXXFLAGS) -fpermissive -o $@ $<

ifeq ($(func), )
	FILTER = *
else
	FILTER = $(func).*
endif

all : $(PROGRAM)
run:
	env LD_LIBRARY_PATH=$(KPGLIBC_modules_DIR)/output/ ./kpglibc_perftest --gtest_filter=$(FILTER)
rrun:
	env LD_LIBRARY_PATH=/usr/local/ksl/lib/ ./kpglibc_perftest --gtest_filter=$(FILTER)

clean :
	@-$(RM) *.d *.a *.so
	@-$(RM)	./$(PROGRAM) $(SRC_DEPS)
	@find $(TEST_modules_DIR)/common/perftest/ -name '*.o' -exec $(RM) {} \;

# Builds a sample test.

# Rules for producing the objects.
#---------------------------------------------------
objs : $(OBJS)

%.o : %.c
	@echo compiling $(notdir $<)
	$(CC_COMPILE)

%.o : %.C
	$(CXX_COMPILE)

%.o : %.cc
	@echo compiling $(notdir $<)
	$(CXX_COMPILE)

%.o : %.cpp
	@echo compiling $(notdir $<)
	$(CXX_COMPILE)

%.o : %.CPP
	$(CXX_COMPILE)

%.o : %.c++
	$(CXX_COMPILE)

%.o : %.cp
	$(CXX_COMPILE)

%.o : %.cxx
	$(CXX_COMPILE)

# Rules for producing the executable.
#----------------------------------------------
$(PROGRAM) : $(OBJS)
ifeq ($(strip $(SRCEXTS)), .c)  # C file
	$(CC)  -o $(PROGRAM)  $(OBJS) $(LD_LIBRARIES)
else                            # C++ file
	@echo Linking $@
	$(SILENCE) $(CXX)  -o $(PROGRAM)  $(OBJS) $(LD_LIBRARIES)
endif

# Include the header dependence list file
-include $(SRC_DEPS)