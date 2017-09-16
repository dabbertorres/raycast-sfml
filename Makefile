NAME := Raycast-SFML

CXX := g++
FLAGS := -std=c++17 -Wall -Wextra
RLS_FLAGS := -O2
DBG_FLAGS := -DDEBUG -O0 -g

LIBS := sfml-system sfml-graphics sfml-window

OUT_DIR := $(realpath .)/out
BUILD_DIR := $(realpath .)/$(NAME)/build

SRC := $(wildcard $(NAME)/*.cpp)
OBJ := $(SRC:%.cpp=%.o)
DEP := $(OBJ:%.o=%.d)

.PHONY: release debug clean

release: $(OUT_DIR)/release/$(NAME)
debug: $(OUT_DIR)/debug/$(NAME)

$(OUT_DIR)/release/$(NAME): $(addprefix $(BUILD_DIR)/release/,$(OBJ))
	$(CXX) $(FLAGS) $(RLS_FLAGS) $^ -o $@ $(addprefix -l,$(LIBS))

$(OUT_DIR)/debug/$(NAME): $(addprefix $(BUILD_DIR)/debug/,$(OBJ))
	$(CXX) $(FLAGS) $(DBG_FLAGS) $^ -o $@ $(addprefix -l,$(addsuffix -d,$(LIBS)))

# generate dependency graphs
-include $(BUILD_DIR)$(DEP)

# -MMD has g++ generate a dependency file for the input file

$(BUILD_DIR)/release/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(FLAGS) $(RLS_FLAGS) -MMD -c $^ -o $@

$(BUILD_DIR)/debug/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(FLAGS) $(DBG_FLAGS) -MMD -c $^ -o $@

clean:
	@rm -rf $(BUILD_DIR) $(OUT_DIR)
