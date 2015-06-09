SUBDIRS = lib src

all: nettrafd

nettrafd: 
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	  (cd $$subdir && $(MAKE)); \
	done;

clean:
	@list='$(SUBDIRS)'; for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) clean); \
	done;
	rm -f *~ *.bak

install: nettrafd
	install src/nettrafd /usr/sbin
	install util/nettraf /etc/rc.d/init.d