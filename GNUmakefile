# --------------------------------------------------------------
# GNUmakefile for XeSim.
# --------------------------------------------------------------

name := XeSim
G4TARGET := $(name)
G4EXLIB := true

ROOTCFLAGS      = $(shell root-config --cflags) -Wno-shadow -w
ROOTLIBS        = $(shell root-config --nonew --libs)
ROOTGLIBS       = $(shell root-config --glibs)

EXTRALIBS +=$(ROOTLIBS)
CPPFLAGS += $(ROOTCFLAGS)

TBBLIBS = -ltbb
EXTRALIBS += $(TBBLIBS)

ifndef G4INSTALL
  G4INSTALL = ../../../..
endif

.PHONY: all
all: lib bin

include $(G4INSTALL)/config/architecture.gmk

include $(G4INSTALL)/config/binmake.gmk
