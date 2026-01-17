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

# Usage python kp_coverage.py
import os
import sys

PROJECT_ROOT = os.path.abspath(os.path.abspath(__file__) + "/../../")
print(PROJECT_ROOT)
lcovcmd = "cd %s;lcov -c -d ./ -o unit-test.info --rc lcov_branch_coverage=1 --exclude '*/clock_gettime.c' --exclude '*/gettimeofday.c' --exclude '*/usr/*' --exclude '*/cpufeature.c' " % PROJECT_ROOT
gencmd = "cd %s;genhtml  unit-test.info -o all --branch-coverage --rc lcov_branch_coverage=1" %PROJECT_ROOT

os.system(lcovcmd)
os.system(gencmd)