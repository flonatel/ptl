GCOV_OUT_DIR=./html

.PHONY: coverage
coverage: #check
	mkdir -p ${GCOV_OUT_DIR}
	lcov -c -b . -d . -o ${GCOV_OUT_DIR}/gcov.info
	lcov -o ${GCOV_OUT_DIR}/gcov.info -r ${GCOV_OUT_DIR}/gcov.info "/usr*"  "*/gmock/*" "*/unittests/*" "*/blackbox/*"
	genhtml ${GCOV_OUT_DIR}/gcov.info -o ${GCOV_OUT_DIR}/gcov/
	@echo coverage report generated successfully, see ${GCOV_OUT_DIR}/gcov/index.html

# Local Variables:
# mode: makefile
# End:
