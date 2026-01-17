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

ARCH = $(shell uname -p | cut -c 1-3)
PWD = $(shell pwd)
ROOT_DIR = $(PWD)/../..
GTEST_DOWNLOAD_DIR = $(ROOT_DIR)/test/gtest-download
GOOGLE_TEST_DIR = $(ROOT_DIR)/test/googletest-release-1.8.0
KPGLIBC_OBJ_DIR = $(ROOT_DIR)/build
KPGLIBC_OUTPUT_DIR = $(ROOT_DIR)/output
REDEF_FILE = $(PWD)/../common/include/*_redefine.h
PERF_TEST_CFG_FILE = $(PWD)/../common/perftest/perf_test.cfg
CMAKE_IN_FILE = $(ROOT_DIR)/config.cmake.in
NO_OMP_RESULT_FILE = no_omp_result.txt
OMP_RESULT_FILE = omp_result.txt
RPM = y
KPGLIBC_INFO_VERSION = $(shell xmllint --xpath "//KPGLIBC_INFO_VERSION/text()" ${ROOT_DIR}/dependence.xml)
KPGLIBC_INFO_COMPONENT_NAME = $(shell xmllint --xpath "//KPGLIBC_INFO_COMPONENT_NAME/text()" ${ROOT_DIR}/dependence.xml)
KPGLIBC_INFO_SOFTWARE_NAME = $(shell xmllint --xpath "//KPGLIBC_INFO_SOFTWARE_NAME/text()" ${ROOT_DIR}/dependence.xml)
KPGLIBC_INFO_PRODUCT_VERSION = $(shell xmllint --xpath "//KPGLIBC_INFO_PRODUCT_VERSION/text()" ${ROOT_DIR}/dependence.xml)

KPGLIBC_C_TOOLCHAIN = $(shell xmllint --xpath "//KPGLIBC_C_TOOLCHAIN/text()" ${ROOT_DIR}/dependence.xml)
export CC=${KPGLIBC_C_TOOLCHAIN}

ifeq ($(start), )
	start = 1
endif

ifeq ($(end), )
	end = 10000
endif

step = $(shell expr \( $(end) - $(start) \) / 10)
line_num1 = $(shell cat $(PERF_TEST_CFG_FILE) | grep -n $(PERF_TEST_LEN) | awk -F":" '{print $$1}')
line_num2 = $(shell cat $(PERF_TEST_CFG_FILE) | grep -n $(PERF_TEST_NUM) | awk -F":" '{print $$1}')
test_len_old = $(shell cat $(PERF_TEST_CFG_FILE) | grep $(PERF_TEST_LEN) | awk -F" " '{print $$2}')
test_num_old = $(shell cat $(PERF_TEST_CFG_FILE) | grep $(PERF_TEST_NUM) | awk -F" " '{print $$2}')
loop_seq = $(shell seq $(start) $(step) $(end))

ifeq ($(func), )
	func = *
endif

ifeq ($(omp), y)
	WITH_OMP = -DBUILD_WITH_OMP=ON
else
	WITH_OMP = -DBUILD_WITH_OMP=OFF
endif

ifeq ($(cover), y)
	WITH_COVER = -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage"
else
ifeq ($(cover), asan)
    WITH_ASAN = -DCMAKE_CXX_FLAGS="-fdump-rtl-expand -fsanitize=address -fprofile-arcs -ftest-coverage"
endif
endif

ifneq ($(fuzzcount), )
    TEST_COUNT = $(fuzzcount)
endif

ifeq ($(ARCH), x86)
	ARCH_OPT = x86
	ARCH_MK_FILE = Makefile.x86
	CSV_FILE_NAME = x86_perf.csv
	OUTDATA_DIR = x86_data
	ipp_func = *

	ipp_num = $(shell grep $(func) $(REDEF_FILE) | awk 'END{print NR}')
	nf = $(shell grep $(func) $(REDEF_FILE) | awk 'NR==1' | awk 'END{printf NF}')

ifneq ($(nf), 3)
	ipp_num = 0
endif

	ipp_func1 = $(shell grep $(func) $(REDEF_FILE) | awk 'NR==1' | awk '{print $$3}')
	nums = $(shell seq 1 128)

ifneq ($(ipp_num), 0)
ifneq ($(ipp_num), 1)
	ipp_func2 = $(shell grep $(func) $(REDEF_FILE) | awk 'NR==2' | awk '{print $$3}')
	part = $(shell for n in $(nums) ;\
	do \
		s1=`echo $(ipp_func1) | cut -c 1-$$n`;\
		s2=`echo $(ipp_func2) | cut -c 1-$$n`;\
		if [ $$s1 != $$s2 ]; then \
			n=`expr $$n - 1`;\
			echo $(ipp_func1) | cut -c 1-$$n;\
			break;\
		fi \
	done)

	final = $(shell echo $(func) | sed 's/\(.*\)\(.\)/\2/')

ifeq ($(final), *)
	ipp_func = $(part)*
else
	ipp_func = $(part)
endif

endif
endif

	diff_func = $(ipp_func)
	is_ipp = $(shell echo $(func) | cut -c 1-3)

ifeq ($(is_ipp), ipp)
	diff_func = $(func)
endif

ifeq ($(ipp_num), 1)
	diff_func = $(ipp_func1)
endif

else
	ARCH_OPT = arm
	ARCH_MK_FILE = Makefile.am
	CSV_FILE_NAME = arm_perf.csv
	OUTDATA_DIR = arm_data
	diff_func = $(func)
endif

all: $(ARCH_OPT)

perf:
	rm -f $(PWD)/$(CSV_FILE_NAME)
	make -C $(ROOT_DIR)/test/build/perftest/ -f $(ARCH_MK_FILE) -j16 omp=$(omp)

func:
	make -C $(ROOT_DIR)/test/build/functest/ -f $(ARCH_MK_FILE) -j16

googletest:
	cd $(GTEST_DOWNLOAD_DIR);\
	sh download.sh;\
	mkdir -p $(GOOGLE_TEST_DIR)/build
	cd $(GOOGLE_TEST_DIR)/build/;\
	cmake $(GOOGLE_TEST_DIR)/;\
	cd $(PWD)/
	make -C $(GOOGLE_TEST_DIR)/build/ -j16

	@cp -u $(GOOGLE_TEST_DIR)/build/googlemock/*.a $(ROOT_DIR)/test/test_tool_bins/gtest/
	@cp -u $(GOOGLE_TEST_DIR)/build/googlemock/gtest/*.a $(ROOT_DIR)/test/test_tool_bins/gtest/

test: googletest
	make func
	make perf

rpmperftest:
	rm -f $(PWD)/$(CSV_FILE_NAME)
	make -C $(ROOT_DIR)/test/build/perftest/ -f $(ARCH_MK_FILE) -j16 rpm=$(RPM)

rpmfunctest:
	make -C $(ROOT_DIR)/test/build/functest/ -f $(ARCH_MK_FILE) -j16 rpm=$(RPM)

rpmtest: googletest
	make rpmfunctest
	make rpmperftest

kpglibclib:
ifeq ("$(cover)", "y")
	sed -i 's/\bTRUE\b/FALSE/g' $(CMAKE_IN_FILE)
endif
	sh $(ROOT_DIR)/build.sh util
	cd $(KPGLIBC_OBJ_DIR)/;\
	cmake $(ROOT_DIR) -DKPGLIBC_TOOLCHAIN_INFO:STRING=${KPGLIBC_C_TOOLCHAIN} \
		-DKPGLIBC_VERSION_INFO=${KPGLIBC_INFO_VERSION} \
		-DKPGLIBC_COMPONENT_NAME:STRING=${KPGLIBC_INFO_COMPONENT_NAME} \
		-DKPGLIBC_SOFTWARE_NAME:STRING=${KPGLIBC_INFO_SOFTWARE_NAME} \
		-DKPGLIBC_PRODUCT_VERSION:STRING=${KPGLIBC_INFO_PRODUCT_VERSION} \
		$(WITH_OMP) $(WITH_COVER) $(WITH_ASAN) $(IPP_2021);\
	make -C $(KPGLIBC_OBJ_DIR)/ -j16

arm: kpglibclib
	make test

x86: test

rpm: rpmtest

rfunc:
	make -C $(ROOT_DIR)/test/build/functest/ -f $(ARCH_MK_FILE) run func=$(diff_func)

rperf:
	make -C $(ROOT_DIR)/test/build/perftest/ -f $(ARCH_MK_FILE) run func=$(diff_func)

run: rfunc rperf

rrfunc:
	make -C $(ROOT_DIR)/test/build/functest/ -f $(ARCH_MK_FILE) rrun func=$(diff_func)

rrperf:
	make -C $(ROOT_DIR)/test/build/perftest/ -f $(ARCH_MK_FILE) rrun func=$(diff_func)

rrun: rrfunc rrperf

fuzz: googletest
	make kpglibclib cover=asan
	cmake -S $(ROOT_DIR)/test/build/fuzztest/ -B $(ROOT_DIR)/test/build/fuzztest/ $(WITH_COVER)
	make -C $(ROOT_DIR)/test/build/fuzztest/ -f Makefile -j16

rfuzz:
	make -C $(ROOT_DIR)/test/build/fuzztest/ -f Makefile run func=$(func)

asan: googletest
	make kpglibclib
	make func

rasan:
	make -C $(ROOT_DIR)/test/build/functest/ -f $(ARCH_MK_FILE) asanrun func=$(diff_func)

omptest:
	@sed -i "$(line_num2)c $(PERF_TEST_NUM) 1" $(PERF_TEST_CFG_FILE)

	rm -f $(PWD)/$(NO_OMP_RESULT_FILE)
	make omp=n;
	@for i in $(loop_seq);\
	do \
		rm -f $(PWD)/$(CSV_FILE_NAME);\
		sed -i "$(line_num1)c $(PERF_TEST_LEN) $$i" $(PERF_TEST_CFG_FILE);\
		make rperf func=$(func);\
		echo test_len=$$i >> $(PWD)/$(NO_OMP_RESULT_FILE);\
		cat $(PWD)/$(CSV_FILE_NAME) | awk -F"," '{print $$1,$$NF}' | grep -v function_name >> $(PWD)/$(NO_OMP_RESULT_FILE);\
	done

	rm -f $(PWD)/$(OMP_RESULT_FILE)
	make omp=y;
	@for i in $(loop_seq);\
	do \
		rm -f $(PWD)/$(CSV_FILE_NAME);\
		sed -i "$(line_num1)c $(PERF_TEST_LEN) $$i" $(PERF_TEST_CFG_FILE);\
		make rperf func=$(func);\
		echo test_len=$$i >> $(PWD)/$(OMP_RESULT_FILE);\
		cat $(PWD)/$(CSV_FILE_NAME) | awk -F"," '{print $$1,$$NF}' | grep -v function_name >> $(PWD)/$(OMP_RESULT_FILE);\
	done

	@sed -i "$(line_num1)c $(PERF_TEST_LEN) ${test_len_old}" $(PERF_TEST_CFG_FILE)
	@sed -i "$(line_num2)c $(PERF_TEST_NUM) ${test_num_old}" $(PERF_TEST_CFG_FILE)

clean:
	make -C $(ROOT_DIR)/test/build/perftest/ -f $(ARCH_MK_FILE) clean
	make -C $(ROOT_DIR)/test/build/functest/ -f $(ARCH_MK_FILE) clean
	@if [ -e "$(ROOT_DIR)/test/build/fuzztest/Makefile" ]; then \
		make -C "$(ROOT_DIR)/test/build/fuzztest" -f Makefile clean_all; \
	fi
	rm -f $(PWD)/$(CSV_FILE_NAME)
	rm -rf $(GOOGLE_TEST_DIR)/build
	rm -rf $(ROOT_DIR)/test/test_tool_bins/gtest/*.a
	rm -f $(PWD)/*.txt

help:
	@echo "Usage:"
	@echo " "
	@echo "make <target> [options]"
	@echo " "
	@echo "To enable incremental compilation, you'd better synchronize your compile machine time to"
	@echo "coding machine, the command is: [sudo date -s 'hh:mm:ss yy-mm-dd']"
	@echo " "
	@echo "Options:"
	@echo "  omp=y              -- make with omp (arm only)"
	@echo "  omp=n              -- make without omp (arm only)"
	@echo "  func=<funcname>    -- run function(s)"
	@echo " "
	@echo "Target:"
	@echo "  all                -- default"
	@echo "  fuzz               -- make fuzztest target"
	@echo "  run                -- run functest and perftest"
	@echo "  rfunc              -- run functest"
	@echo "  rperf              -- run perftest"
	@echo "  rfuzz              -- run fuzztest"
	@echo "  clean              -- clean project"
	@echo " "
	@echo "Examples:"
	@echo "  make                       -- make all target"
	@echo "  make omp=y                 -- make with omp (arm only)"
	@echo "  make omp=n                 -- make without omp (arm only)"
	@echo "  make run func=*            -- run all functest and perftest case"
	@echo "  make rfunc func=funcname   -- run a functest case"
	@echo "  make rperf func=funcname*  -- run a series of perftest case"
	@echo "  make readi func=funcname   -- read input test data"
	@echo "  make reado func=funcname   -- read output test data"
	@echo "  make omptest start=1 end=100 func=funcname   -- do omp test"
