-include extra.mk
-include buildsys.mk

SUBDIRS=$(LIBMOWGLI) modules src
CLEANDIRS = ${SUBDIRS}

install-extra:
	@echo "----------------------------------------------------------------"
	@echo ">>> Remember to cd to ${prefix} and edit your config file.";
	@echo "----------------------------------------------------------------"

buildsys.mk:
	@echo "You need to run ./configure first."
	@exit 1
