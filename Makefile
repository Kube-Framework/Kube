# Kube's Makefile helper
# Used to prevent misusage of cmake

# Predefined build directories
RELEASE_DIR			=	Release
DEBUG_DIR			=	Debug

# Build type and directory
BUILD_DIR			=	$(RELEASE_DIR)
BUILD_TYPE			=	Release

# Coverage output
COVERAGE_OUTPUT		=	coverage.info

# Coverage file format to extract (see run_coverage_extract)
COVERAGE_EXTRACT	=

# Cmake arguments stack
CMAKE_ARGS			=

# Commands
RM					=	rm -rf

# General build rules
all: release

build:
	cmake -E make_directory $(BUILD_DIR)
	cmake -B $(BUILD_DIR) -H. -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) ${CMAKE_ARGS} -GNinja
	cmake --build $(BUILD_DIR)

release:
	$(MAKE) build BUILD_DIR=$(RELEASE_DIR) BUILD_TYPE=Release

debug:
	$(MAKE) build BUILD_DIR=$(DEBUG_DIR) BUILD_TYPE=Debug

# General tests rules
tests:
	$(MAKE) release CMAKE_ARGS+=-DKF_TESTS=ON

tests_debug:
	$(MAKE) debug CMAKE_ARGS+=-DKF_TESTS=ON

run_tests: tests
	ninja -C $(RELEASE_DIR) test

run_tests_debug: tests_debug
	ninja -C $(DEBUG_DIR) test

# General code coverage rules
coverage:
	$(MAKE) tests_debug CMAKE_ARGS+="-DKF_COVERAGE=ON"

run_coverage: coverage
	ninja -C $(DEBUG_DIR) test
	lcov --directory . --capture --no-external --output-file $(COVERAGE_OUTPUT)
	lcov --remove $(COVERAGE_OUTPUT) "*/Tests/*" -o $(COVERAGE_OUTPUT)
	lcov --list $(COVERAGE_OUTPUT)

run_coverage_extract: coverage
	./Debug/Kube$(COVERAGE_EXTRACT)Tests
	lcov --directory . --capture --no-external --output-file $(COVERAGE_OUTPUT)
	lcov --remove $(COVERAGE_OUTPUT) "*/Tests/*" -o $(COVERAGE_OUTPUT)
	lcov --extract $(COVERAGE_OUTPUT) "*/$(COVERAGE_EXTRACT)/*" -o $(COVERAGE_OUTPUT)
	lcov --list $(COVERAGE_OUTPUT)

# General benchmarks rules
benchmarks:
	$(MAKE) release CMAKE_ARGS+=-DKF_BENCHMARKS=ON

benchmarks_debug:
	$(MAKE) debug CMAKE_ARGS+=-DKF_BENCHMARKS=ON

# Examples rules
examples:
	$(MAKE) release CMAKE_ARGS+=-DKF_EXAMPLES=ON

examples_debug:
	$(MAKE) debug CMAKE_ARGS+=-DKF_EXAMPLES=ON


# Audio
AUDIO_ARGS = CMAKE_ARGS+=-DKF_AUDIO=ON
audio:
	$(MAKE) release $(AUDIO_ARGS)

audio_debug:
	$(MAKE) debug $(AUDIO_ARGS)

audio_tests:
	$(MAKE) tests $(AUDIO_ARGS)

run_audio_tests:
	$(MAKE) run_tests $(AUDIO_ARGS)

audio_tests_debug:
	$(MAKE) tests_debug $(AUDIO_ARGS)

run_audio_tests_debug:
	$(MAKE) run_tests_debug $(AUDIO_ARGS)

audio_coverage:
	$(MAKE) coverage $(AUDIO_ARGS)

run_audio_coverage:
	$(MAKE) run_coverage_extract $(AUDIO_ARGS) COVERAGE_EXTRACT="Audio"

audio_benchmarks:
	$(MAKE) benchmarks $(AUDIO_ARGS)

audio_benchmarks_debug:
	$(MAKE) benchmarks_debug $(AUDIO_ARGS)


# App
APP_ARGS = CMAKE_ARGS+=-DKF_APP=ON
app:
	$(MAKE) release $(APP_ARGS)

app_debug:
	$(MAKE) debug $(APP_ARGS)

app_tests:
	$(MAKE) tests $(APP_ARGS)

run_app_tests:
	$(MAKE) run_tests $(APP_ARGS)

app_tests_debug:
	$(MAKE) tests_debug $(APP_ARGS)

run_app_tests_debug:
	$(MAKE) run_tests_debug $(APP_ARGS)

app_coverage:
	$(MAKE) coverage $(APP_ARGS)

run_app_coverage:
	$(MAKE) run_coverage_extract $(APP_ARGS) COVERAGE_EXTRACT="App"

app_benchmarks:
	$(MAKE) benchmarks $(APP_ARGS)

app_benchmarks_debug:
	$(MAKE) benchmarks_debug $(APP_ARGS)


# Core
CORE_ARGS = CMAKE_ARGS+=-DKF_CORE=ON
core:
	$(MAKE) release $(CORE_ARGS)

core_debug:
	$(MAKE) debug $(CORE_ARGS)

core_tests:
	$(MAKE) tests $(CORE_ARGS)

run_core_tests:
	$(MAKE) run_tests $(CORE_ARGS)

core_tests_debug:
	$(MAKE) tests_debug $(CORE_ARGS)

run_core_tests_debug:
	$(MAKE) run_tests_debug $(CORE_ARGS)

core_coverage:
	$(MAKE) coverage $(CORE_ARGS)

run_core_coverage:
	$(MAKE) run_coverage_extract $(CORE_ARGS) COVERAGE_EXTRACT="Core"

core_benchmarks:
	$(MAKE) benchmarks $(CORE_ARGS)

core_benchmarks_debug:
	$(MAKE) benchmarks_debug $(CORE_ARGS)


# ECS
ECS_ARGS = CMAKE_ARGS+=-DKF_ECS=ON
ecs:
	$(MAKE) release $(ECS_ARGS)

ecs_debug:
	$(MAKE) debug $(ECS_ARGS)

ecs_tests:
	$(MAKE) tests $(ECS_ARGS)

run_ecs_tests:
	$(MAKE) run_tests $(ECS_ARGS)

ecs_tests_debug:
	$(MAKE) tests_debug $(ECS_ARGS)

run_ecs_tests_debug:
	$(MAKE) run_tests_debug $(ECS_ARGS)

ecs_coverage:
	$(MAKE) coverage $(ECS_ARGS)

run_ecs_coverage:
	$(MAKE) run_coverage_extract $(ECS_ARGS) COVERAGE_EXTRACT="ECS"

ecs_benchmarks:
	$(MAKE) benchmarks $(ECS_ARGS)

ecs_benchmarks_debug:
	$(MAKE) benchmarks_debug $(ECS_ARGS)


# Graphics
GRAPHICS_ARGS = CMAKE_ARGS+=-DKF_GRAPHICS=ON
graphics:
	$(MAKE) release $(GRAPHICS_ARGS)

graphics_debug:
	$(MAKE) debug $(GRAPHICS_ARGS)

graphics_tests:
	$(MAKE) tests $(GRAPHICS_ARGS)

run_graphics_tests:
	$(MAKE) run_tests $(GRAPHICS_ARGS)

graphics_tests_debug:
	$(MAKE) tests_debug $(GRAPHICS_ARGS)

run_graphics_tests_debug:
	$(MAKE) run_tests_debug $(GRAPHICS_ARGS)

graphics_coverage:
	$(MAKE) coverage $(GRAPHICS_ARGS)

run_graphics_coverage:
	$(MAKE) run_coverage_extract $(GRAPHICS_ARGS) COVERAGE_EXTRACT="Graphics"

graphics_benchmarks:
	$(MAKE) benchmarks $(GRAPHICS_ARGS)

graphics_benchmarks_debug:
	$(MAKE) benchmarks_debug $(GRAPHICS_ARGS)


# Interpreter
INTERPRETER_ARGS = CMAKE_ARGS+=-DKF_INTERPRETER=ON
interpreter:
	$(MAKE) release $(INTERPRETER_ARGS)

interpreter_debug:
	$(MAKE) debug $(INTERPRETER_ARGS)

interpreter_tests:
	$(MAKE) tests $(INTERPRETER_ARGS)

run_interpreter_tests:
	$(MAKE) run_tests $(INTERPRETER_ARGS)

interpreter_tests_debug:
	$(MAKE) tests_debug $(INTERPRETER_ARGS)

run_interpreter_tests_debug:
	$(MAKE) run_tests_debug $(INTERPRETER_ARGS)

interpreter_coverage:
	$(MAKE) coverage $(INTERPRETER_ARGS)

run_interpreter_coverage:
	$(MAKE) run_coverage_extract $(INTERPRETER_ARGS) COVERAGE_EXTRACT="Interpreter"

interpreter_benchmarks:
	$(MAKE) benchmarks $(INTERPRETER_ARGS)

interpreter_benchmarks_debug:
	$(MAKE) benchmarks_debug $(INTERPRETER_ARGS)


# Meta
META_ARGS = CMAKE_ARGS+=-DKF_META=ON
meta:
	$(MAKE) release $(META_ARGS)

meta_debug:
	$(MAKE) debug $(META_ARGS)

meta_tests:
	$(MAKE) tests $(META_ARGS)

run_meta_tests:
	$(MAKE) run_tests $(META_ARGS)

meta_tests_debug:
	$(MAKE) tests_debug $(META_ARGS)

run_meta_tests_debug:
	$(MAKE) run_tests_debug $(META_ARGS)

meta_coverage:
	$(MAKE) coverage $(META_ARGS)

run_meta_coverage:
	$(MAKE) run_coverage_extract $(META_ARGS) COVERAGE_EXTRACT="Meta"

meta_benchmarks:
	$(MAKE) benchmarks $(META_ARGS)

meta_benchmarks_debug:
	$(MAKE) benchmarks_debug $(META_ARGS)


# Object
OBJECT_ARGS = CMAKE_ARGS+=-DKF_OBJECT=ON
object:
	$(MAKE) release $(OBJECT_ARGS)

object_debug:
	$(MAKE) debug $(OBJECT_ARGS)

object_tests:
	$(MAKE) tests $(OBJECT_ARGS)

run_object_tests:
	$(MAKE) run_tests $(OBJECT_ARGS)

object_tests_debug:
	$(MAKE) tests_debug $(OBJECT_ARGS)

run_object_tests_debug:
	$(MAKE) run_tests_debug $(OBJECT_ARGS)

object_coverage:
	$(MAKE) coverage $(OBJECT_ARGS)

run_object_coverage:
	$(MAKE) run_coverage_extract $(OBJECT_ARGS) COVERAGE_EXTRACT="Object"

object_benchmarks:
	$(MAKE) benchmarks $(OBJECT_ARGS)

object_benchmarks_debug:
	$(MAKE) benchmarks_debug $(OBJECT_ARGS)

# UI
UI_ARGS = CMAKE_ARGS+=-DKF_UI=ON
ui:
	$(MAKE) release $(UI_ARGS)

ui_debug:
	$(MAKE) debug $(UI_ARGS)

ui_tests:
	$(MAKE) tests $(UI_ARGS)

run_ui_tests:
	$(MAKE) run_tests $(UI_ARGS)

ui_tests_debug:
	$(MAKE) tests_debug $(UI_ARGS)

run_ui_tests_debug:
	$(MAKE) run_tests_debug $(UI_ARGS)

ui_coverage:
	$(MAKE) coverage $(UI_ARGS)

run_ui_coverage:
	$(MAKE) run_coverage_extract $(UI_ARGS) COVERAGE_EXTRACT="UI"

ui_benchmarks:
	$(MAKE) benchmarks $(UI_ARGS)

ui_benchmarks_debug:
	$(MAKE) benchmarks_debug $(UI_ARGS)


# Flow
FLOW_ARGS = CMAKE_ARGS+=-DKF_FLOW=ON
flow:
	$(MAKE) release $(FLOW_ARGS)

flow_debug:
	$(MAKE) debug $(FLOW_ARGS)

flow_tests:
	$(MAKE) tests $(FLOW_ARGS)

run_flow_tests:
	$(MAKE) run_tests $(FLOW_ARGS)

flow_tests_debug:
	$(MAKE) tests_debug $(FLOW_ARGS)

run_flow_tests_debug:
	$(MAKE) run_tests_debug $(FLOW_ARGS)

flow_coverage:
	$(MAKE) coverage $(FLOW_ARGS)

run_flow_coverage:
	$(MAKE) run_coverage_extract $(FLOW_ARGS) COVERAGE_EXTRACT="Flow"

flow_benchmarks:
	$(MAKE) benchmarks $(FLOW_ARGS)

flow_benchmarks_debug:
	$(MAKE) benchmarks_debug $(FLOW_ARGS)

# Voxel
VOXEL_ARGS = CMAKE_ARGS+=-DKF_VOXEL=ON
voxel:
	$(MAKE) release $(VOXEL_ARGS)

voxel_debug:
	$(MAKE) debug $(VOXEL_ARGS)

voxel_tests:
	$(MAKE) tests $(VOXEL_ARGS)

run_voxel_tests:
	$(MAKE) run_tests $(VOXEL_ARGS)

voxel_tests_debug:
	$(MAKE) tests_debug $(VOXEL_ARGS)

run_voxel_tests_debug:
	$(MAKE) run_tests_debug $(VOXEL_ARGS)

voxel_coverage:
	$(MAKE) coverage $(VOXEL_ARGS)

run_voxel_coverage:
	$(MAKE) run_coverage_extract $(VOXEL_ARGS) COVERAGE_EXTRACT="Voxel"

voxel_benchmarks:
	$(MAKE) benchmarks $(VOXEL_ARGS)

voxel_benchmarks_debug:
	$(MAKE) benchmarks_debug $(VOXEL_ARGS)


# Widgets
WIDGETS_ARGS = CMAKE_ARGS+=-DKF_WIDGETS=ON
widgets:
	$(MAKE) release $(WIDGETS_ARGS)

widgets_debug:
	$(MAKE) debug $(WIDGETS_ARGS)

widgets_tests:
	$(MAKE) tests $(WIDGETS_ARGS)

run_widgets_tests:
	$(MAKE) run_tests $(WIDGETS_ARGS)

widgets_tests_debug:
	$(MAKE) tests_debug $(WIDGETS_ARGS)

run_widgets_tests_debug:
	$(MAKE) run_tests_debug $(WIDGETS_ARGS)

widgets_coverage:
	$(MAKE) coverage $(WIDGETS_ARGS)

run_widgets_coverage:
	$(MAKE) run_coverage_extract $(WIDGETS_ARGS) COVERAGE_EXTRACT="Widgets"

widgets_benchmarks:
	$(MAKE) benchmarks $(WIDGETS_ARGS)

widgets_benchmarks_debug:
	$(MAKE) benchmarks_debug $(WIDGETS_ARGS)

# Cleaning rules
clean:
	$(RM) ${RELEASE_DIR}
	$(RM) ${DEBUG_DIR}

fclean: clean

# Clear and rebuild everything in release
re: clean all

.PHONY: all \
	release debug \
	examples examples_debug \
	tests tests_debug run_tests run_tests_debug \
	benchmarks benchmarks_debug \
	audio audio_debug audio_tests run_audio_tests audio_tests_debug run_audio_tests_debug audio_coverage run_audio_coverage audio_benchmarks audio_benchmarks_debug \
	app app_debug app_tests run_app_tests app_tests_debug run_app_tests_debug app_coverage run_app_coverage app_benchmarks app_benchmarks_debug \
	core core_debug core_tests run_core_tests core_tests_debug run_core_tests_debug core_coverage run_core_coverage core_benchmarks core_benchmarks_debug \
	graphics graphics_debug graphics_tests run_graphics_tests graphics_tests_debug run_graphics_tests_debug graphics_coverage run_graphics_coverage graphics_benchmarks graphics_benchmarks_debug \
	interpreter interpreter_debug interpreter_tests run_interpreter_tests interpreter_tests_debug run_interpreter_tests_debug interpreter_coverage run_interpreter_coverage interpreter_benchmarks interpreter_benchmarks_debug \
	meta meta_debug meta_tests run_meta_tests meta_tests_debug run_meta_tests_debug meta_coverage run_meta_coverage meta_benchmarks meta_benchmarks_debug \
	ui ui_debug ui_tests run_ui_tests ui_tests_debug run_ui_tests_debug ui_coverage run_ui_coverage ui_benchmarks ui_benchmarks_debug \
	flow flow_debug flow_tests run_flow_tests flow_tests_debug run_flow_tests_debug flow_coverage run_flow_coverage flow_benchmarks flow_benchmarks_debug \
	widgets widgets_debug widgets_tests run_widgets_tests widgets_tests_debug run_widgets_tests_debug widgets_coverage run_widgets_coverage widgets_benchmarks widgets_benchmarks_debug \
	clean fclean \
	re