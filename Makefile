SRC_TYPES := -iname '*.h' -o -iname '*.hh' -o -iname '*.hpp' -o -iname '*.c' -o -iname '*.cc' -o -iname '*.cpp' -o -iname '*.cxx'
CMD_FORMAT := clang-format -i


## clean: Cleanup build outputs
.PHONY: clean
clean:
	@echo "  >  Cleaning up..."
	@rm -rf ./bin



## fmt: Formats source files
.PHONY: fmt
fmt:
	@echo "  >  Formatting..."
	@find ./src/dsaac -type f $(SRC_TYPES) | xargs $(CMD_FORMAT)
	@find ./src/epoll_examples -type f $(SRC_TYPES) | xargs $(CMD_FORMAT)
	@find ./src/medsr -type f $(SRC_TYPES) | xargs $(CMD_FORMAT)
	@find ./src/texas -type f $(SRC_TYPES) | xargs $(CMD_FORMAT)
	@find ./src/tlpi -type f $(SRC_TYPES) | xargs $(CMD_FORMAT)
