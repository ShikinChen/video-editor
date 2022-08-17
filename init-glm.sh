#! /usr/bin/env bash
#
# Copyright (C) 2013-2015 Bilibili
# Copyright (C) 2013-2015 Zhang Rui <bbcallen@gmail.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


IJK_GLM_UPSTREAM=https://github.com/g-truc/glm
IJK_GLM_FORK=https://github.com/g-truc/glm
IJK_GLM_LOCAL_REPO=extra/glm

set -e
TOOLS=tools

echo "== pull glm base =="
sh $TOOLS/pull-repo-base.sh $IJK_GLM_UPSTREAM $IJK_GLM_LOCAL_REPO

echo "== pull glm fork =="
cd $IJK_GLM_LOCAL_REPO
git checkout 0.9.9.8 -b 0.9.9.8
cd -
