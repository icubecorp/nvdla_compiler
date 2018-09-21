# Copyright (c) 2017-2018, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

LOCAL_DIR := $(GET_LOCAL_DIR)

NVDLA_SRC_FILES := \
	allocator.cpp \
	layer.cpp \
	mat.cpp \
	modelbin.cpp \
	net.cpp \
	paramdict.cpp \
	blob.cpp \
	main.cpp \
	debug.cpp \
    	net_parser.cpp \
	nvdla_layer/conv.cpp \
	nvdla_layer/pdp.cpp \
	nvdla_layer/sdp.cpp \
	nvdla_layer/nv_input.cpp \
	nvdla_layer/nv_softmax.cpp \
	caffe_layer/input.cpp \
	caffe_layer/relu.cpp \
    	caffe_layer/softmax.cpp \
    	caffe_layer/innerproduct.cpp \
    	caffe_layer/pooling.cpp \
	caffe_layer/convolution.cpp \
	list_parser/list_entry_parser.cpp \
	list_parser/task_list_parser.cpp \
	list_parser/submit_list_parser.cpp \
	list_parser/memory_list_parser.cpp \
	list_parser/address_list_parser.cpp \
	list_parser/event_list_parser.cpp \
	list_parser/tensor_desc_list_parser.cpp \
	list_parser/symbol_list_parser.cpp \
	list_parser/reloc_list_parser.cpp 

NVDLA_SRC_FILES += \
	caffe_to_flatbuf.cpp \
	$(ROOT)/core/common/Loadable.cpp \
	$(ROOT)/core/common/Check.cpp \
	$(ROOT)/core/common/ErrorLogging.c 
	

INCLUDES += \
    -I$(ROOT)/include \
    -I$(ROOT)/core/include \
    -I$(ROOT)/core/common/include \
    -I$(ROOT)/external/include \
    -I$(ROOT)/external/libjpeg-turbo \
    -I$(LOCAL_DIR)/include \
    -I$(LOCAL_DIR)/nvdla_layer \
    -I$(LOCAL_DIR)

MODULE_CPPFLAGS += -DNVDLA_UTILS_ERROR_TAG="\"DLA_TEST\""
MODULE_CFLAGS += -DNVDLA_UTILS_ERROR_TAG="\"DLA_TEST\""

#SHARED_LIBS := \
#    $(ROOT)/out/runtime/libnvdla_runtime/libnvdla_runtime.so
SHARED_LIBS := 

MODULE_SRCS := $(NVDLA_SRC_FILES)

include $(ROOT)/make/module.mk
