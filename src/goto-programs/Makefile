SRC = goto_convert.cpp goto_convert_function_call.cpp \
      goto_convert_side_effect.cpp goto_program.cpp basic_blocks.cpp \
      goto_convert_exceptions.cpp property_checker.cpp \
      remove_function_pointers.cpp goto_functions.cpp goto_inline.cpp \
      remove_skip.cpp goto_convert_functions.cpp string_instrumentation.cpp \
      builtin_functions.cpp show_properties.cpp set_properties.cpp \
      read_goto_binary.cpp goto_asm.cpp elf_reader.cpp \
      string_abstraction.cpp destructor.cpp remove_asm.cpp \
      read_bin_goto_object.cpp goto_program_irep.cpp interpreter.cpp \
      interpreter_evaluate.cpp json_goto_trace.cpp \
      format_strings.cpp loop_ids.cpp pointer_arithmetic.cpp \
      goto_program_template.cpp write_goto_binary.cpp remove_unreachable.cpp \
      remove_unused_functions.cpp remove_vector.cpp \
      wp.cpp goto_clean_expr.cpp safety_checker.cpp parameter_assignments.cpp \
      compute_called_functions.cpp link_to_library.cpp \
      remove_returns.cpp osx_fat_reader.cpp remove_complex.cpp \
      goto_trace.cpp xml_goto_trace.cpp vcd_goto_trace.cpp graphml_goto_trace.cpp

INCLUDES= -I ..

include ../config.inc
include ../common

CLEANFILES = goto-programs$(LIBEXT)

all: goto-programs$(LIBEXT)

###############################################################################

goto-programs$(LIBEXT): $(OBJ)
	$(LINKLIB)
