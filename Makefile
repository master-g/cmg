# cmg Makefile —— 统一功能入口
#
# 用法: make <target> [TARGET=dsaac] [BUILD_TYPE=Debug]
#
# 前置条件:
#   - cmake >= 3.16、C 编译器 (clang / gcc)
#   - clang-format (仅 fmt 需要)
#
# 注意: epoll_examples 只在 Linux 可构建，macOS 上 `make build` 会停在这里，
#       用 `make build TARGET=<子项目>` 绕开。

CMAKE ?= cmake
CMD_FORMAT := clang-format -i

# BUILD_DIR: cmake 构建目录，与产物目录 bin/ 分开
BUILD_DIR ?= build
# BUILD_TYPE: Debug | Release | RelWithDebInfo | MinSizeRel
BUILD_TYPE ?= Debug
# TARGET: all | dsaac | landlord | medsr | mph | texas_eval | texas_all | texas_generate |
#         texas_test | osm_eval | osm_genarray | copy | epoll_examples
TARGET ?= all

SRC_TYPES := -iname '*.h' -o -iname '*.hh' -o -iname '*.hpp' -o -iname '*.c' -o -iname '*.cc' -o -iname '*.cpp' -o -iname '*.cxx'
FMT_DIRS := ./src/dsaac ./src/epoll_examples ./src/landlord ./src/medsr ./src/texas ./src/tlpi

.DEFAULT_GOAL := help

.PHONY: help configure build test fmt clean clean-build ename

help: ## 显示本帮助
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-16s\033[0m %s\n", $$1, $$2}'

configure: ## 生成 cmake 构建系统
	$(CMAKE) -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

build: configure ## 构建 (TARGET=all 时 macOS 会停在 epoll_examples)
	$(CMAKE) --build $(BUILD_DIR) --target $(TARGET)

test: ## 跑 assert 自检: dsaac 与 texas_test
	@$(MAKE) build TARGET=dsaac
	@$(MAKE) build TARGET=texas_test
	./bin/dsaac
	./bin/texas_test

fmt: ## 用 clang-format 格式化源文件
	@echo "  >  Formatting..."
	@find $(FMT_DIRS) -type f $(SRC_TYPES) | xargs $(CMD_FORMAT)

ename: ## 重新生成 src/libs/tlpi/ename.c.inc
	cd src/libs/tlpi && sh Build_ename.sh > ename.c.inc

clean: ## 清理可执行产物 bin/
	@echo "  >  Cleaning up..."
	@rm -rf ./bin

clean-build: clean ## 清理 bin/ 和 cmake 构建目录
	@rm -rf $(BUILD_DIR)
