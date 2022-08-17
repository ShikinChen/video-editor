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

set -e
TOOLS=tools

git --version

X264_UPSTREAM=https://code.videolan.org/videolan/x264.git
X264_FORK=https://code.videolan.org/videolan/x264.git
X264_LOCAL_REPO=extra/x264

sh $TOOLS/pull-repo-base.sh $X264_UPSTREAM $X264_LOCAL_REPO

function pull_fork_x264()
{
    sh $TOOLS/pull-repo-ref.sh $X264_FORK android/contrib/x264-$1 ${X264_LOCAL_REPO}
    cd android/contrib/x264-$1
    # git checkout stable -B stable
    git checkout baee400f -B baee400f
    cd -
}

pull_fork_x264 "armv7a"
pull_fork_x264 "arm64"
pull_fork_x264 "x86"
pull_fork_x264 "x86_64"
