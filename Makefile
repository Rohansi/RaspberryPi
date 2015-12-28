AS          = arm-none-eabi-as
ASFLAGS     = $(ARCH)
CC          =
CFLAGS      =
CXX         = arm-none-eabi-g++
CXXBARE     = -ffreestanding -fno-builtin -nostdlib -fno-exceptions -fno-rtti -fno-stack-protector -nostdinc++
CXXSTD      = -std=c++14 -Wall -Wextra -Wpedantic -Werror -O3
CXXFLAGS    = -c $(foreach INCDIR, $(INCDIRS), -I$(INCDIR)) $(CXXSTD) $(CXXBARE) $(ARCH)
LD          = arm-none-eabi-ld
LDFLAGS     =
LIBGCC      = /opt/cross/lib/gcc/arm-none-eabi/5.3.0/libgcc.a
QAS         = vc4asm
QASFLAGS    = -V

ARCH        = -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard

SRCDIR      = source
INCDIRS     = include $(QPUDIR)
BINDIR      = bin
OBJDIR      = obj
QPUDIR      = $(OBJDIR)/qpu

HEADERS     = $(foreach DIR, $(INCDIRS), $(wildcard $(DIR)/*.h))
SOURCES     = $(foreach DIR, $(SRCDIR), $(wildcard $(DIR)/*.s $(DIR)/*.cpp))
OBJECTS     = $(foreach OBJECT, $(patsubst %.s, %.o, $(patsubst %.cpp, %.o, $(SOURCES))), $(OBJDIR)/$(OBJECT))

QPUSOURCES  = $(foreach DIR, $(SRCDIR), $(wildcard $(DIR)/*.qasm))
QPUOBJECTS  = $(foreach OBJECT, $(patsubst %.qasm, %.qpu.inc, $(QPUSOURCES)), $(QPUDIR)/$(OBJECT))

TARGETELF   = $(OBJDIR)/kernel.elf
TARGET      = $(BINDIR)/kernel.img

.PHONY : all clean

all : qpu $(TARGET)

clean :
	@rm -rf $(BINDIR) $(OBJDIR)

qpu : $(QPUOBJECTS)

$(TARGETELF) : $(OBJECTS)
	$(info LNK $@)
	@mkdir -p $(@D)
	@$(LD) $(LDFLAGS) -o $@ -T source/kernel.ld $+ $(LIBGCC)

$(TARGET): $(TARGETELF)
	$(info IMG $@)
	@mkdir -p $(@D)
	@arm-none-eabi-objcopy $< -O binary $@

$(OBJDIR)/%.o : %.s
	$(info ASM $<)
	@mkdir -p $(@D)
	@$(AS) $(ASFLAGS) $< -o $@

$(OBJDIR)/%.o : %.cpp
	$(info CPP $<)
	@mkdir -p $(@D)
	@$(CXX) $(CXXFLAGS) $< -o $@
    
$(QPUDIR)/%.qpu.inc : %.qasm
	$(info QPU $<)
	@mkdir -p $(QPUDIR)
	@$(QAS) $(QASFLAGS) -C $(QPUDIR)/$(@F) $< 
